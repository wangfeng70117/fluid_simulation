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
//��������
Particle* particleArray = NULL;
//Particle* nextParticleArray = NULL;
//��Ԫ������
Box* boxArray = NULL;
//����������
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
    //�ӿ�
    glViewport(0, 0, width, height);

    //����ͶӰ����
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    //glLoadIdentity();  //���ص�λ����
    gluLookAt(0, 0.5, -0.5, 0, 0, 0, 0, 1, 0);
    //�����ӿڱ���  ʹͼ�����Ŵ���ѹ�����仯
    //�����Ӿ���  ���ǻ��ǻ�����???
    gluPerspective(45.0f, width / height, 0.1f, 100.0f);

    //ģ�;���
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glShadeModel(GL_SMOOTH);
    //������ɫ
    glClearColor(0.75f, 0.75f, 0.85f, 1.0f);
}
//���Ƶ�
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
//�����������ܵ�����
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
        //������������1  ���Ծ�ʡ��������
        float sum = h6; //�Ȱ��Լ����ܶȼ���
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

    glm::vec3 force1(0, 0, 0); //ѹ��
    glm::vec3 force2(0, 0, 0); //ճ����

    for (int i = 0; i < particleNumber; i++) 
    {
        Particle currentParticle = particleArray[i];
        //������������1  ���Ծ�ʡ��������
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
                    //����ѹ�����ٶ�

                    vdiff = neighborPartical.velocity - particleArray[i].velocity; //�ٶȵ�΢��
                    //ѹ��
                    float param = (particleArray[i].intensityPressure + neighborPartical.intensityPressure)
                        * 0.5 * constant.pressureKernel * 
                        (r_h) * (r_h) / distance;
                    //force1 = diff * (constant.mass * param / (particleArray[i].density * neighborPartical.density));
                    force1 = r * (constant.mass * param / (particleArray[i].density * neighborPartical.density));
                    //ճ����
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
    //���������±߽�
    if (newPosition.y <= min_y)
    {
        //�±߽編����
        glm::vec3 n(0, 1, 0);
        glm::vec3 v1 = particleArray[index].velocity;
        v1.y *= 0.2;
        //��ײ����ٶ�
        particleArray[index].velocity = v1 - 2 * (glm::dot(v1,n)) * n;
        return true;
    }
    //���������ϱ߽�
    if (newPosition.y >= max_y)
    {
        //�±߽編����
        glm::vec3 n(0, -1, 0);
        glm::vec3 v1 = particleArray[index].velocity;
        v1.y *= 0.2;
        //��ײ����ٶ�
        particleArray[index].velocity = v1 - 2 * (glm::dot(v1, n)) * n;
        return true;
        //��ײ����ٶ�
        //particleArray[index].velocity = -constant.normalDragCoefficient *
        //    (particle->velocity * normal) * normal +
        //    constant.tangentDragCoefficient * (particle->velocity - Vnormal);
    }
    //����������߽�
    if (newPosition.x <= min_x)
    {
        glm::vec3 n(1, 0, 0);
        glm::vec3 v1 = particleArray[index].velocity;
        v1 *= 0.2;
        //��ײ����ٶ�
        particleArray[index].velocity = v1 - 2 * (glm::dot(v1, n)) * n;
        return true;
    }
    //���������ұ߽�
    if (newPosition.x >= max_x)
    {
        glm::vec3 n(-1, 0, 0);
        glm::vec3 v1 = particleArray[index].velocity;
        v1 *= 0.2;
        //��ײ����ٶ�
        particleArray[index].velocity = v1 - 2 * (glm::dot(v1, n)) * n;
        return true;
    }
    //��������ǰ�߽�
    if (newPosition.z <= min_z)
    {
        glm::vec3 n(0, 0, 1);
        glm::vec3 v1 = particleArray[index].velocity;
        v1 *= 0.2;
        //��ײ����ٶ�
        particleArray[index].velocity = v1 - 2 * (glm::dot(v1, n)) * n;
        return true;
    }
    //����������߽�
    if (newPosition.z >= max_z)
    {
        glm::vec3 n(0, 0, -1);
        glm::vec3 v1 = particleArray[index].velocity;
        v1 *= 0.2;
        //��ײ����ٶ�
        particleArray[index].velocity = v1 - 2 * (glm::dot(v1, n)) * n;
        return true;
    }
    return false;
}
//�����ƶ�
void ParticleMove(float deltaTime) {

    for (int i = 0; i < particleNumber; i++) {
        //���Ӽ��ٶ� a = f / m
        Particle particle = particleArray[i];
        glm::vec3 acceleration = particle.acceleration;
        //�����µ��ٶ� v = v0 + at
        particleArray[i].velocity = particle.velocity + acceleration * deltaTime;
        //�����µ�λ�� p = p0 + vt

        glm::vec3 newPosition = particle.position +
            (particleArray[i].velocity + particle.velocity) * deltaTime / 2.0f;

        //�ƶ����������ں��ӵı��
        int boxIndex = box.getBoxIndex(particle.position);
        if (boxIndex < 0 || boxIndex > box.gridNumber - 1)
            continue;
        //ԭ���ӱ��
        int initBoxIndex = particle.boxIndex;
        //������Ӵ�Խ�˺���,ԭ����ɾ�����ӱ��   �µĺ��Ӳ������ӱ��
        if (initBoxIndex != boxIndex)
        {
            //�ɵĺ���ɾ����ǰ����
            boxArray[initBoxIndex].deleteParticle(i);
            //���ӵĺ��ӱ�Ÿĳ��µĺ��ӱ��
            particle.boxIndex = boxIndex;
            //�µĺ�����ӵ�ǰ����
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