#include <iostream>
//#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath>

// إعدادات النافذة
const int WIDTH = 800, HEIGHT = 600;

// كود Vertex Shader مع //n'
const char* vertexShaderSource =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"out vec3 Position;\n"
"void main() {\n"
"    gl_Position = vec4(aPos, 1.0);\n"
"    Position = aPos;\n"
"}\n";

// كود Fragment Shader مع //n'
const char* fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 Position;\n"
"\n"
"uniform float uTime;\n"
"uniform vec3 uFrameColor;\n"
"uniform float uFrameZ;\n"
"\n"
"void main() {\n"
"    // المسافة من المركز\n"
"    float dist = sqrt(Position.x * Position.x + Position.y * Position.y);\n"
"    \n"
"    // إطار دائري رفيع جداً\n"
"    float frameRadius = 0.75;\n"
"    float frameWidth = 0.02;\n"
"    \n"
"    if (dist > frameRadius - frameWidth && dist < frameRadius + frameWidth) {\n"
"        // تأثير Z على اللون\n"
"        float zEffect = (uFrameZ + 1.0) / 2.0;\n"
"        \n"
"        // اختفاء تدريجي\n"
"        float alpha = 1.0;\n"
"        if (uFrameZ < -0.3f) {\n"
"            alpha = 1.0 - (abs(uFrameZ) - 0.3f) * 1.5f;\n"
"            if (alpha < 0.0) alpha = 0.0;\n"
"        }\n"
"        \n"
"        FragColor = vec4(uFrameColor * (0.5 + zEffect * 0.5), alpha);\n"
"        return;\n"
"    }\n"
"    \n"
"    // داخل الساعة\n"
"    if (dist < frameRadius - frameWidth) {\n"
"        // خلفية زرقاء غامقة\n"
"        FragColor = vec4(0.0, 0.0, 0.15, 1.0);\n"
"        \n"
"        // حساب زوايا العقارب من الوقت\n"
"        float seconds = uTime;\n"
"        float minutes = uTime / 60.0;\n"
"        float hours = uTime / 3600.0;\n"
"        \n"
"        // تحويل إلى زوايا (2π = دورة كاملة)\n"
"        float secondsAngle = seconds * 2.0 * 3.14159;\n"
"        float minutesAngle = minutes * 2.0 * 3.14159;\n"
"        float hoursAngle = hours * 2.0 * 3.14159;\n"
"        \n"
"        // رسم نقاط الساعات (12 نقطة)\n"
"        for (int i = 0; i < 12; i++) {\n"
"            float angle = i * 2.0 * 3.14159 / 12.0;\n"
"            float x = sin(angle) * 0.6;\n"
"            float y = cos(angle) * 0.6;\n"
"            \n"
"            float dotDist = sqrt((Position.x - x) * (Position.x - x) + \n"
"                                (Position.y - y) * (Position.y - y));\n"
"            if (dotDist < 0.03) {\n"
"                FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
"                return;\n"
"            }\n"
"        }\n"
"        \n"
"        // رسم عقرب الثواني (أحمر)\n"
"        float sx = sin(secondsAngle) * 0.6;\n"
"        float sy = cos(secondsAngle) * 0.6;\n"
"        \n"
"        float t = ((Position.x * sx + Position.y * sy) / (sx * sx + sy * sy));\n"
"        if (t > 0 && t < 1) {\n"
"            float projx = t * sx;\n"
"            float projy = t * sy;\n"
"            float distToLine = sqrt((Position.x - projx) * (Position.x - projx) + \n"
"                                   (Position.y - projy) * (Position.y - projy));\n"
"            if (distToLine < 0.015) {\n"
"                FragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
"                return;\n"
"            }\n"
"        }\n"
"        \n"
"        // رسم عقرب الدقائق (أزرق)\n"
"        float mx = sin(minutesAngle) * 0.5;\n"
"        float my = cos(minutesAngle) * 0.5;\n"
"        \n"
"        t = ((Position.x * mx + Position.y * my) / (mx * mx + my * my));\n"
"        if (t > 0 && t < 1) {\n"
"            float projx = t * mx;\n"
"            float projy = t * my;\n"
"            float distToLine = sqrt((Position.x - projx) * (Position.x - projx) + \n"
"                                   (Position.y - projy) * (Position.y - projy));\n"
"            if (distToLine < 0.02) {\n"
"                FragColor = vec4(0.0, 0.0, 1.0, 1.0);\n"
"                return;\n"
"            }\n"
"        }\n"
"        \n"
"        // رسم عقرب الساعات (أخضر)\n"
"        float hx = sin(hoursAngle) * 0.4;\n"
"        float hy = cos(hoursAngle) * 0.4;\n"
"        \n"
"        t = ((Position.x * hx + Position.y * hy) / (hx * hx + hy * hy));\n"
"        if (t > 0 && t < 1) {\n"
"            float projx = t * hx;\n"
"            float projy = t * hy;\n"
"            float distToLine = sqrt((Position.x - projx) * (Position.x - projx) + \n"
"                                   (Position.y - projy) * (Position.y - projy));\n"
"            if (distToLine < 0.025) {\n"
"                FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
"                return;\n"
"            }\n"
"        }\n"
"        \n"
"        // نقطة المنتصف\n"
"        if (dist < 0.04) {\n"
"            FragColor = vec4(1.0, 1.0, 1.0, 1.0);\n"
"            return;\n"
"        }\n"
"    }\n"
"}\n";

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
    float speed = 1.0f;

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

        // التحكم بسرعة الوقت
        if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS) {
            speed += 0.1f;
        }
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS) {
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
    }

    return 0;
}
