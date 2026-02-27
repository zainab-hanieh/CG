#include <iostream>
//#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>

// إعدادات النافذة
const int WIDTH = 1000, HEIGHT = 800;

// كود Vertex Shader
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
out vec3 Position;

void main() {
    gl_Position = vec4(aPos, 1.0);
    Position = aPos;
}
)";

// كود Fragment Shader
const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
in vec3 Position;

uniform float uTime;           // الوقت من glfwGetTime()
uniform vec3 uFrameColor;      // لون الإطار
uniform float uFrameZ;         // قيمة Z للإطار (تقريب وتبعيد)

void main() {
    // المسافة من المركز
    float dist = sqrt(Position.x * Position.x + Position.y * Position.y);
    
    // **إطار دائري رفيع جداً** (سمك 0.02 فقط)
    float frameRadius = 0.75;
    float frameWidth = 0.02; // أرق من الأول
    
    // الإطار الدائري
    if (dist > frameRadius - frameWidth && dist < frameRadius + frameWidth) {
        // تأثير Z على اللون
        float zEffect = (uFrameZ + 1.0) / 2.0;
        
        // اختفاء تدريجي
        float alpha = 1.0;
        if (uFrameZ < -0.3f) {
            alpha = 1.0 - (abs(uFrameZ) - 0.3f) * 1.5f;
            if (alpha < 0.0) alpha = 0.0;
        }
        
        FragColor = vec4(uFrameColor * (0.5 + zEffect * 0.5), alpha);
        return;
    }
    
    // داخل الساعة
    if (dist < frameRadius - frameWidth) {
        // خلفية زرقاء غامقة
        FragColor = vec4(0.0, 0.0, 0.15, 1.0);
        
        // **حساب زوايا العقارب من الوقت**
        float seconds = uTime;
        float minutes = uTime / 60.0;
        float hours = uTime / 3600.0;
        
        // تحويل إلى زوايا (2π = دورة كاملة)
        float secondsAngle = seconds * 2.0 * 3.14159; // دورة كل 60 ثانية
        float minutesAngle = minutes * 2.0 * 3.14159; // دورة كل 60 دقيقة
        float hoursAngle = hours * 2.0 * 3.14159;     // دورة كل 12 ساعة
        
        // رسم نقاط الساعات (12 نقطة)
        for (int i = 0; i < 12; i++) {
            float angle = i * 2.0 * 3.14159 / 12.0;
            float x = sin(angle) * 0.6;
            float y = cos(angle) * 0.6;
            
            float dotDist = sqrt((Position.x - x) * (Position.x - x) + 
                                (Position.y - y) * (Position.y - y));
            if (dotDist < 0.03) {
                FragColor = vec4(1.0, 1.0, 1.0, 1.0);
                return;
            }
        }
        
        // **عقرب الثواني (أحمر) - يتحرك تلقائياً**
        float sx = sin(secondsAngle) * 0.6;
        float sy = cos(secondsAngle) * 0.6;
        
        float t = ((Position.x * sx + Position.y * sy) / (sx * sx + sy * sy));
        if (t > 0 && t < 1) {
            float projx = t * sx;
            float projy = t * sy;
            float distToLine = sqrt((Position.x - projx) * (Position.x - projx) + 
                                   (Position.y - projy) * (Position.y - projy));
            if (distToLine < 0.015) {
                FragColor = vec4(1.0, 0.0, 0.0, 1.0);
                return;
            }
        }
        
        // **عقرب الدقائق (أزرق) - يتحرك تلقائياً**
        float mx = sin(minutesAngle) * 0.5;
        float my = cos(minutesAngle) * 0.5;
        
        t = ((Position.x * mx + Position.y * my) / (mx * mx + my * my));
        if (t > 0 && t < 1) {
            float projx = t * mx;
            float projy = t * my;
            float distToLine = sqrt((Position.x - projx) * (Position.x - projx) + 
                                   (Position.y - projy) * (Position.y - projy));
            if (distToLine < 0.02) {
                FragColor = vec4(0.0, 0.0, 1.0, 1.0);
                return;
            }
        }
        
        // **عقرب الساعات (أخضر) - يتحرك تلقائياً**
        float hx = sin(hoursAngle) * 0.4;
        float hy = cos(hoursAngle) * 0.4;
        
        t = ((Position.x * hx + Position.y * hy) / (hx * hx + hy * hy));
        if (t > 0 && t < 1) {
            float projx = t * hx;
            float projy = t * hy;
            float distToLine = sqrt((Position.x - projx) * (Position.x - projx) + 
                                   (Position.y - projy) * (Position.y - projy));
            if (distToLine < 0.025) {
                FragColor = vec4(0.0, 1.0, 0.0, 1.0);
                return;
            }
        }
        
        // نقطة المنتصف
        if (dist < 0.04) {
            FragColor = vec4(1.0, 1.0, 1.0, 1.0);
            return;
        }
    }
}
)";

// بيانات الرؤوس
float vertices[] = {
    -1.0f, -1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f
};

unsigned int indices[] = {
    0, 1, 2,
    2, 3, 0
};

int main() {
    // تهيئة GLFW
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "الساعة التلقائية", nullptr, nullptr);
    if (!window) return -1;

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    // إنشاء VAO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // إنشاء الشيدرز
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // مواقع Uniform
    GLint timeLoc = glGetUniformLocation(shaderProgram, "uTime");
    GLint frameColorLoc = glGetUniformLocation(shaderProgram, "uFrameColor");
    GLint frameZLoc = glGetUniformLocation(shaderProgram, "uFrameZ");

    // متغيرات التحكم
    float frameColor[3] = { 1.0f, 1.0f, 0.0f }; // أصفر
    float frameZ = 0.0f;
    float speed = 1.0f; // سرعة الوقت (1 = طبيعي)

    while (!glfwWindowShouldClose(window)) {
        // معالجة المدخلات
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // تغيير لون الإطار
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            frameColor[0] = 1.0f; frameColor[1] = 0.0f; frameColor[2] = 0.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
            frameColor[0] = 0.0f; frameColor[1] = 1.0f; frameColor[2] = 0.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
            frameColor[0] = 0.0f; frameColor[1] = 0.0f; frameColor[2] = 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
            frameColor[0] = 1.0f; frameColor[1] = 1.0f; frameColor[2] = 0.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            frameColor[0] = 1.0f; frameColor[1] = 1.0f; frameColor[2] = 1.0f;
        }

        // التحكم بسرعة الوقت
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) { // زائد
            speed += 0.1f;
        }
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) { // ناقص
            speed -= 0.1f;
            if (speed < 0.1f) speed = 0.1f;
        }

        // تقريب وتبعيد الإطار
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
            frameZ += 0.02f;
            if (frameZ > 1.0f) frameZ = 1.0f;
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            frameZ -= 0.02f;
            if (frameZ < -1.0f) frameZ = -1.0f;
        }

        // تنظيف الشاشة
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // تحديث Uniforms
        float time = glfwGetTime() * speed;
        glUniform1f(timeLoc, time);
        glUniform3f(frameColorLoc, frameColor[0], frameColor[1], frameColor[2]);
        glUniform1f(frameZLoc, frameZ);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();

        // عرض المعلومات
        std::cout << "Z: " << frameZ << " | Speed: " << speed << "x  \r";
    }

    return 0;
}