#include <iostream>

// 1. تضمين المكتبات
// ملاحظة: يجب تضمين GLEW قبل GLFW دائماً
//#define GLEW_STATIC // لأننا نستخدم مكتبة glew32s.lib (Static)
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// إعدادات النافذة
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// --- 2. كود المظللات (Shaders) ---

// A. كود Vertex Shader مع مصفوفة التحويل
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 ourColor;\n"
"uniform mat4 transform;\n"
"void main()\n"
"{\n"
"   gl_Position = transform * vec4(aPos, 1.0);\n"
"   ourColor = aColor;\n"
"}\0";

// B. كود Fragment Shader للتدرج اللوني
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 ourColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(ourColor, 1.0f);\n"
"}\n\0";

// دالة لمعالجة تغيير حجم النافذة
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// دالة لمعالجة المدخلات
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    // --- 3. تهيئة GLFW وإعداد النافذة ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // إنشاء كائن النافذة
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Colored Angled Rectangle and Triangle", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // --- 4. تهيئة GLEW ---
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // --- 5. بناء وتجميع برنامج الشيدر (Shader Program) ---

    // تجميع Vertex Shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // تجميع Fragment Shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // ربط الشيدرز في برنامج واحد
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // حذف الشيدرز المنفصلة
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // --- 6. تعريف البيانات للمستطيل المائل (Angled Rectangle) ---
    float rectangleVertices[] = {
        // المواقع (شكل معين مائل)      // الألوان
        -0.1f,  0.1f, 0.0f,             1.0f, 0.0f, 0.0f, // أحمر - أعلى يسار
         0.4f,  0.3f, 0.0f,             0.0f, 1.0f, 0.0f, // أخضر - أعلى يمين
         0.2f, -0.2f, 0.0f,             0.0f, 0.0f, 1.0f, // أزرق - أسفل يمين
        -0.3f, -0.4f, 0.0f,             1.0f, 1.0f, 0.0f  // أصفر - أسفل يسار
    };

    unsigned int rectangleIndices[] = {
        0, 1, 2,  // المثلث الأول
        0, 2, 3   // المثلث الثاني
    };

    // --- 7. تعريف البيانات للمثلث (Triangle) بجانب المستطيل ---
    float triangleVertices[] = {
        // المواقع (على يسار المستطيل)  // الألوان
        -0.8f, -0.2f, 0.0f,             1.0f, 0.0f, 0.0f, // أحمر
        -0.5f, -0.2f, 0.0f,             0.0f, 1.0f, 0.0f, // أخضر
        -0.65f, 0.3f, 0.0f,             0.0f, 0.0f, 1.0f  // أزرق
    };

    // --- 8. إعداد VAO, VBO, EBO للمستطيل المائل ---
    unsigned int rectangleVAO, rectangleVBO, rectangleEBO;
    glGenVertexArrays(1, &rectangleVAO);
    glGenBuffers(1, &rectangleVBO);
    glGenBuffers(1, &rectangleEBO);

    glBindVertexArray(rectangleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rectangleEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangleIndices), rectangleIndices, GL_STATIC_DRAW);

    // الموقع (attribute 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // اللون (attribute 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // --- 9. إعداد VAO, VBO للمثلث ---
    unsigned int triangleVAO, triangleVBO;
    glGenVertexArrays(1, &triangleVAO);
    glGenBuffers(1, &triangleVBO);

    glBindVertexArray(triangleVAO);

    glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

    // الموقع (attribute 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // اللون (attribute 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // فك الربط
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // --- 10. حلقة الرسم (Render Loop) ---
    while (!glfwWindowShouldClose(window))
    {
        // معالجة المدخلات
        processInput(window);

        // تنظيف الشاشة
        glClearColor(0.1f, 0.1f, 0.2f, 1.0f); // لون أزرق داكن للخلفية
        glClear(GL_COLOR_BUFFER_BIT);

        // تفعيل برنامج الشيدر
        glUseProgram(shaderProgram);

        // الحصول على موقع متغير transform في الشيدر
        int transformLoc = glGetUniformLocation(shaderProgram, "transform");

        // ------------------------------------------------------------
        // رسم المستطيل المائل (Angled Rectangle)
        // ------------------------------------------------------------
        float rectangleTransform[16] = {
            1.0f, 0.2f, 0.0f, 0.0f,  // Shear in x-direction
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.3f, 0.0f, 0.0f, 1.0f   // Translation to the right
        };

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, rectangleTransform);

        glBindVertexArray(rectangleVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // ------------------------------------------------------------
        // رسم المثلث (Triangle) بجانب المستطيل
        // ------------------------------------------------------------
        float triangleTransform[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            -0.2f, 0.1f, 0.0f, 1.0f  // Translation to position it next to the rectangle
        };

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, triangleTransform);

        glBindVertexArray(triangleVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // تبديل الـ Buffers ومعالجة الأحداث
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // --- 11. التنظيف النهائي ---
    glDeleteVertexArrays(1, &rectangleVAO);
    glDeleteBuffers(1, &rectangleVBO);
    glDeleteBuffers(1, &rectangleEBO);
    glDeleteVertexArrays(1, &triangleVAO);
    glDeleteBuffers(1, &triangleVBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}