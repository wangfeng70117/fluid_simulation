#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include "glm/glm.hpp"
#include <math.h>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "Particle.h"
#include "COLOR.h"
#include "Box.h"
#include "Constant.h"
//粒子数组
Particle* particleArray = NULL;
//Particle* nextParticleArray = NULL;
//单元格数组
Box* boxArray = NULL;
//粒子总数量
Constant constant;
int particleNumber = constant.gridSize * constant.gridSize * constant.gridSize;
int width = 640;
int height = 480;
float min_x = constant.min_x;
float min_y = constant.min_y;
float min_z = constant.min_z;
float max_x = constant.max_x;
float max_y = constant.max_y;
float max_z = constant.max_z;
Box box;

void GenParticlePosition()
{

    particleArray = new Particle[particleNumber];
    //nextParticleArray = new Particle[particleNumber];
    boxArray = new Box[box.gridNumber];

    for (int i = 0; i < box.gridNumber; i++) 
    {
        boxArray[i].init();
    }
    int index;
    for(int i = 0 ;i < constant.gridSize; i++)
        for(int j = 0; j < constant.gridSize; j++)
            for (int k = 0; k < constant.gridSize; k++)
            {
                index = i * constant.gridSize * constant.gridSize + j * constant.gridSize + k;
                particleArray[index].position = glm::vec3
                (
                    (float)(i - constant.gridSize / 2) * constant.initDistance,
                    (float)(j - constant.gridSize / 2) * constant.initDistance,
                    (float)(k - constant.gridSize / 2) * constant.initDistance
                );
            }
    for (int i = 0; i < particleNumber; i++) {
        particleArray[i].velocity = glm::vec3();

        int boxIndex = box.getBoxIndex(particleArray[i].position);
        particleArray[i].boxIndex = boxIndex;
        boxArray[boxIndex].insertParticle(i);
    }
}


void setMVPMatrix() {
    //视口
    glViewport(0, 0, width, height);

    //设置投影矩阵
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //glLoadIdentity();  //加载单位矩阵
    gluLookAt(0, 0.5, -0.5, 0, 0, 0, 0, 1, 0);
    //设置视口比例  使图像不随着窗口压缩而变化
    //控制视景体  但是还是会拉伸???
    gluPerspective(45.0f, width / height, 0.1f, 100.0f);

    //模型矩阵
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glShadeModel(GL_SMOOTH);
    //背景颜色
    glClearColor(0.75f, 0.75f, 0.85f, 1.0f);
}
//绘制点
void drawPoint() {
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(0.0f, 0, -0.15f);

    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(2.0f);
    glBegin(GL_POINTS);
    {
        for (int i = 0; i < particleNumber; ++i)
            glVertex3fv(glm::value_ptr(particleArray[i].position));
    }
    glEnd();
}

void drawBorder() 
{
    glColor3f(0.0f, 0.0f, 1.0f);

    //bring the lines closer to prevent z-fighting with triangles
    glPushMatrix();
    //glTranslatef(0.0f, 0.0f, 0.5f);

    glBegin(GL_LINES);
    {
        
        glVertex3f(min_x, min_y, min_z);
        glVertex3f(max_x, min_y, min_z);

        glVertex3f(min_x, max_y, min_z);
        glVertex3f(max_x, max_y, min_z);

        glVertex3f(min_x, max_y, min_z);
        glVertex3f(min_x, min_y, min_z);

        glVertex3f(max_x, min_y, min_z);
        glVertex3f(max_x, max_y, min_z);

        glVertex3f(min_x, min_y, max_z);
        glVertex3f(max_x, min_y, max_z);

        glVertex3f(min_x, max_y, max_z);
        glVertex3f(max_x, max_y, max_z);

        glVertex3f(min_x, max_y, max_z);
        glVertex3f(min_x, min_y, max_z);

        glVertex3f(max_x, min_y, max_z);
        glVertex3f(max_x, max_y, max_z);

        glVertex3f(min_x, max_y, min_z);
        glVertex3f(min_x, max_y, max_z);

        glVertex3f(max_x, max_y, min_z);
        glVertex3f(max_x, max_y, max_z);

        glVertex3f(min_x, min_y, min_z);
        glVertex3f(min_x, min_y, max_z);

        glVertex3f(max_x, min_y, min_z);
        glVertex3f(max_x, min_y, max_z);
    }
    glEnd();

    glPopMatrix();
}
//计算粒子所受到的力
void calculateParticleProperty() 
{
    for (int i = 0; i < particleNumber; ++i)
    {
        particleArray[i].density = 0;
        particleArray[i].acceleration = glm::vec3(0, -3.0, 0);
    }
    float h6 = pow(constant.smoothRadius, 6);
    for (int i = 0; i < particleNumber; i++)
    {
        Particle currentParticle = particleArray[i];
        //所有质量都是1  所以就省略了质量
        float sum = h6; //先把自己的密度加上
        std::vector<int> allGridIndex = box.getALLGridIndex(particleArray[i].position);
        for (int j = 0; j < allGridIndex.size(); j++)
        {
            if (allGridIndex[j] < 0 || allGridIndex[j] > box.gridNumber - 1)
                continue;
            std::vector<int> particleVector = boxArray[allGridIndex[j]].getParticleIndex();

            int boxParticleNum = particleVector.size();
            if (boxParticleNum == 0)
                continue;

            for (int particleNum = 0; particleNum < boxParticleNum; particleNum++)
            {
                int particleIndex = particleVector[particleNum];
                Particle neighborPartical = particleArray[particleIndex];
                glm::vec3 r = currentParticle.position - neighborPartical.position;
                float distance = glm::length(r);
                if (distance == 0)
                    continue;
                if (distance < constant.smoothRadius)
                {
                    float h2_r2 = constant.smoothRadius * constant.smoothRadius -
                        distance * distance;
                    sum += pow(h2_r2, 3);
                }
            }
        }

        particleArray[i].density = sum * constant.densityKernel * 3.1;
        particleArray[i].intensityPressure = constant.pressutr_k * (particleArray[i].density - 1000.0f);
    }

    glm::vec3 vdiff;
    glm::vec3 diff;

    glm::vec3 force1(0, 0, 0); //压力
    glm::vec3 force2(0, 0, 0); //粘滞力

    for (int i = 0; i < particleNumber; i++) 
    {
        Particle currentParticle = particleArray[i];
        //所有质量都是1  所以就省略了质量
        std::vector<int> allGridIndex = box.getALLGridIndex(particleArray[i].position);
        for (int j = 0; j < allGridIndex.size(); j++)
        {
            if (allGridIndex[j] < 0 || allGridIndex[j] > box.gridNumber - 1)
                continue;
            std::vector<int> particleVector = boxArray[allGridIndex[j]].getParticleIndex();

            int boxParticleNum = particleVector.size();
            if (boxParticleNum == 0)
                continue;
            for (int particleNum = 0; particleNum < boxParticleNum; particleNum++) 
            {
                int particleIndex = particleVector[particleNum];
                Particle neighborPartical = particleArray[particleIndex];
                glm::vec3 r = currentParticle.position - neighborPartical.position;
                float distance = glm::length(r);
                if (distance == 0)
                    continue;
                if (distance < constant.smoothRadius)
                {

                    float r_h = constant.smoothRadius - distance;
                    //计算压力加速度

                    vdiff = neighborPartical.velocity - particleArray[i].velocity; //速度的微分
                    //压力
                    float param = (particleArray[i].intensityPressure + neighborPartical.intensityPressure)
                        * 0.5 * constant.pressureKernel * 
                        (r_h) * (r_h) / distance;
                    //force1 = diff * (constant.mass * param / (particleArray[i].density * neighborPartical.density));
                    force1 = r * (constant.mass * param / (particleArray[i].density * neighborPartical.density));
                    //粘滞力
                    force2 = vdiff * (constant.mass * r_h * 0.2f * constant.pressureKernel / (particleArray[i].density
                        * neighborPartical.density));

                    particleArray[i].acceleration += force1;
                    particleArray[i].acceleration += force2;
                }  
            }
        }
    }
}


bool CollisionDetection(int index,glm::vec3& newPosition)
{
    float minDistance = 0.0001f;
    //v2 = v1 - 2(v1 * n) *  n
    //粒子碰到下边界
    if (newPosition.y <= min_y)
    {
        //下边界法向量
        glm::vec3 n(0, 1, 0);
        glm::vec3 v1 = particleArray[index].velocity;
        v1.y *= 0.2;
        //碰撞后的速度
        particleArray[index].velocity = v1 - 2 * (glm::dot(v1,n)) * n;
        return true;
    }
    //粒子碰到上边界
    if (newPosition.y >= max_y)
    {
        //下边界法向量
        glm::vec3 n(0, -1, 0);
        glm::vec3 v1 = particleArray[index].velocity;
        v1.y *= 0.2;
        //碰撞后的速度
        particleArray[index].velocity = v1 - 2 * (glm::dot(v1, n)) * n;
        return true;
        //碰撞后的速度
        //particleArray[index].velocity = -constant.normalDragCoefficient *
        //    (particle->velocity * normal) * normal +
        //    constant.tangentDragCoefficient * (particle->velocity - Vnormal);
    }
    //粒子碰到左边界
    if (newPosition.x <= min_x)
    {
        glm::vec3 n(1, 0, 0);
        glm::vec3 v1 = particleArray[index].velocity;
        v1 *= 0.2;
        //碰撞后的速度
        particleArray[index].velocity = v1 - 2 * (glm::dot(v1, n)) * n;
        return true;
    }
    //粒子碰到右边界
    if (newPosition.x >= max_x)
    {
        glm::vec3 n(-1, 0, 0);
        glm::vec3 v1 = particleArray[index].velocity;
        v1 *= 0.2;
        //碰撞后的速度
        particleArray[index].velocity = v1 - 2 * (glm::dot(v1, n)) * n;
        return true;
    }
    //粒子碰到前边界
    if (newPosition.z <= min_z)
    {
        glm::vec3 n(0, 0, 1);
        glm::vec3 v1 = particleArray[index].velocity;
        v1 *= 0.2;
        //碰撞后的速度
        particleArray[index].velocity = v1 - 2 * (glm::dot(v1, n)) * n;
        return true;
    }
    //粒子碰到后边界
    if (newPosition.z >= max_z)
    {
        glm::vec3 n(0, 0, -1);
        glm::vec3 v1 = particleArray[index].velocity;
        v1 *= 0.2;
        //碰撞后的速度
        particleArray[index].velocity = v1 - 2 * (glm::dot(v1, n)) * n;
        return true;
    }
    return false;
}
//粒子移动
void ParticleMove(float deltaTime) {

    for (int i = 0; i < particleNumber; i++) {
        //粒子加速度 a = f / m
        Particle particle = particleArray[i];
        glm::vec3 acceleration = particle.acceleration;
        //粒子新的速度 v = v0 + at
        particleArray[i].velocity = particle.velocity + acceleration * deltaTime;
        //粒子新的位置 p = p0 + vt

        glm::vec3 newPosition = particle.position +
            (particleArray[i].velocity + particle.velocity) * deltaTime / 2.0f;

        //移动后粒子所在盒子的编号
        int boxIndex = box.getBoxIndex(particle.position);
        if (boxIndex < 0 || boxIndex > box.gridNumber - 1)
            continue;
        //原盒子编号
        int initBoxIndex = particle.boxIndex;
        //如果粒子穿越了盒子,原盒子删除粒子编号   新的盒子插入粒子编号
        if (initBoxIndex != boxIndex)
        {
            //旧的盒子删除当前粒子
            boxArray[initBoxIndex].deleteParticle(i);
            //粒子的盒子编号改成新的盒子编号
            particle.boxIndex = boxIndex;
            //新的盒子添加当前粒子
            boxArray[boxIndex].insertParticle(i);
        }
        if (!CollisionDetection(i, newPosition))
            particleArray[i].position = newPosition;
    }
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(5);

    std::cout << "min_x: " << min_x << " min_y:" << min_y <<
        " min_z: " << min_z <<
        " max_x: " << max_x <<
        " max_y: " << max_y <<
        " max_z: " << max_y << std::endl;
    GenParticlePosition();
    setMVPMatrix();
    while (!glfwWindowShouldClose(window))
    {
        drawPoint();
        drawBorder();
        calculateParticleProperty();
        ParticleMove(0.03f);
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}