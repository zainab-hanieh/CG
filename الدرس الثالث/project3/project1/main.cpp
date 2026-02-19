#include <iostream>
//#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cmath> // for sin/cos

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// ==========================================
// GLOBAL VARIABLES FOR CONTROL
// ==========================================
// Shape 1 (Red Triangle) properties
float shape1PosX = -0.3f;
float shape1PosY = 0.0f;
float shape1PosZ = 0.0f;
float shape1Rotation = 0.0f;
float shape1Scale = 1.0f;
float shape1Alpha = 0.6f;
float shape1Color[3] = { 1.0f, 0.0f, 0.0f }; // Red

// Shape 2 (Blue Triangle) properties
float shape2PosX = 0.3f;
float shape2PosY = 0.0f;
float shape2PosZ = 0.5f;
float shape2Rotation = 0.0f;
float shape2Scale = 1.0f;
float shape2Alpha = 1.0f;
float shape2Color[3] = { 0.0f, 0.0f, 1.0f }; // Blue

// Control modes
enum ControlMode {
    MODE_POSITION_X,
    MODE_POSITION_Y,
    MODE_POSITION_Z,
    MODE_ROTATION,
    MODE_SCALE,
    MODE_ALPHA,
    MODE_COLOR_R,
    MODE_COLOR_G,
    MODE_COLOR_B
};

int currentMode1 = MODE_POSITION_X;
int currentMode2 = MODE_POSITION_X;

// ==========================================
// VERTEX SHADER
// ==========================================
const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 vertexColor;\n"
"uniform mat4 transform;\n"
"void main()\n"
"{\n"
"   gl_Position = transform * vec4(aPos, 1.0);\n"
"   vertexColor = aColor;\n"
"}\0";

// ==========================================
// FRAGMENT SHADER
// ==========================================
const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec3 vertexColor;\n"
"uniform vec3 uColor;\n"
"uniform float uAlpha;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(uColor, uAlpha);\n"
"}\n\0";

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// ==========================================
// Function to create transformation matrix
// ==========================================
void createTransformMatrix(float* matrix, float posX, float posY, float posZ,
    float rotX, float rotY, float rotZ, float scale)
{
    // Identity matrix
    for (int i = 0; i < 16; i++) {
        matrix[i] = 0.0f;
    }
    matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;

    // Scale
    matrix[0] *= scale;
    matrix[5] *= scale;
    matrix[10] *= scale;

    // Rotation Z
    float c = cos(rotZ);
    float s = sin(rotZ);
    matrix[0] = scale * c;
    matrix[1] = scale * s;
    matrix[4] = scale * -s;
    matrix[5] = scale * c;

    // Translation
    matrix[12] = posX;
    matrix[13] = posY;
    matrix[14] = posZ;
}

// ==========================================
// Input Processing
// ==========================================
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Shape 1 mode selection
    static bool keyPressed1 = false, keyPressed2 = false, keyPressed3 = false,
        keyPressed4 = false, keyPressed5 = false, keyPressed6 = false,
        keyPressed7 = false, keyPressed8 = false, keyPressed9 = false;

    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !keyPressed1) {
        currentMode1 = MODE_POSITION_X;
        std::cout << "Shape 1: Controlling X Position (current: " << shape1PosX << ")" << std::endl;
        keyPressed1 = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) keyPressed1 = false;

    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !keyPressed2) {
        currentMode1 = MODE_POSITION_Y;
        std::cout << "Shape 1: Controlling Y Position (current: " << shape1PosY << ")" << std::endl;
        keyPressed2 = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) keyPressed2 = false;

    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !keyPressed3) {
        currentMode1 = MODE_POSITION_Z;
        std::cout << "Shape 1: Controlling Z Position (depth) (current: " << shape1PosZ << ")" << std::endl;
        keyPressed3 = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) keyPressed3 = false;

    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && !keyPressed4) {
        currentMode1 = MODE_ROTATION;
        std::cout << "Shape 1: Controlling Rotation (current: " << shape1Rotation << ")" << std::endl;
        keyPressed4 = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) keyPressed4 = false;

    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && !keyPressed5) {
        currentMode1 = MODE_SCALE;
        std::cout << "Shape 1: Controlling Scale (current: " << shape1Scale << ")" << std::endl;
        keyPressed5 = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE) keyPressed5 = false;

    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && !keyPressed6) {
        currentMode1 = MODE_ALPHA;
        std::cout << "Shape 1: Controlling Transparency (current: " << shape1Alpha << ")" << std::endl;
        keyPressed6 = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_RELEASE) keyPressed6 = false;

    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && !keyPressed7) {
        currentMode1 = MODE_COLOR_R;
        std::cout << "Shape 1: Controlling Red Color (current: " << shape1Color[0] << ")" << std::endl;
        keyPressed7 = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_RELEASE) keyPressed7 = false;

    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && !keyPressed8) {
        currentMode1 = MODE_COLOR_G;
        std::cout << "Shape 1: Controlling Green Color (current: " << shape1Color[1] << ")" << std::endl;
        keyPressed8 = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_RELEASE) keyPressed8 = false;

    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && !keyPressed9) {
        currentMode1 = MODE_COLOR_B;
        std::cout << "Shape 1: Controlling Blue Color (current: " << shape1Color[2] << ")" << std::endl;
        keyPressed9 = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_RELEASE) keyPressed9 = false;

    // Adjust Shape 1 with arrow keys
    float speed = 0.01f;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        switch (currentMode1) {
        case MODE_POSITION_X: shape1PosX += speed; break;
        case MODE_POSITION_Y: shape1PosY += speed; break;
        case MODE_POSITION_Z: shape1PosZ += speed; break;
        case MODE_ROTATION: shape1Rotation += 0.02f; break;
        case MODE_SCALE: shape1Scale += 0.01f; break;
        case MODE_ALPHA: shape1Alpha = fmin(1.0f, shape1Alpha + 0.01f); break;
        case MODE_COLOR_R: shape1Color[0] = fmin(1.0f, shape1Color[0] + 0.01f); break;
        case MODE_COLOR_G: shape1Color[1] = fmin(1.0f, shape1Color[1] + 0.01f); break;
        case MODE_COLOR_B: shape1Color[2] = fmin(1.0f, shape1Color[2] + 0.01f); break;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        switch (currentMode1) {
        case MODE_POSITION_X: shape1PosX -= speed; break;
        case MODE_POSITION_Y: shape1PosY -= speed; break;
        case MODE_POSITION_Z: shape1PosZ -= speed; break;
        case MODE_ROTATION: shape1Rotation -= 0.02f; break;
        case MODE_SCALE: shape1Scale = fmax(0.1f, shape1Scale - 0.01f); break;
        case MODE_ALPHA: shape1Alpha = fmax(0.0f, shape1Alpha - 0.01f); break;
        case MODE_COLOR_R: shape1Color[0] = fmax(0.0f, shape1Color[0] - 0.01f); break;
        case MODE_COLOR_G: shape1Color[1] = fmax(0.0f, shape1Color[1] - 0.01f); break;
        case MODE_COLOR_B: shape1Color[2] = fmax(0.0f, shape1Color[2] - 0.01f); break;
        }
    }

    // Shape 2 mode selection
    static bool keyPressedS1 = false, keyPressedS2 = false, keyPressedS3 = false,
        keyPressedS4 = false, keyPressedS5 = false, keyPressedS6 = false,
        keyPressedS7 = false, keyPressedS8 = false, keyPressedS9 = false;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !keyPressedS1) {
            currentMode2 = MODE_POSITION_X;
            std::cout << "Shape 2: Controlling X Position (current: " << shape2PosX << ")" << std::endl;
            keyPressedS1 = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE) keyPressedS1 = false;

        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !keyPressedS2) {
            currentMode2 = MODE_POSITION_Y;
            std::cout << "Shape 2: Controlling Y Position (current: " << shape2PosY << ")" << std::endl;
            keyPressedS2 = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE) keyPressedS2 = false;

        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && !keyPressedS3) {
            currentMode2 = MODE_POSITION_Z;
            std::cout << "Shape 2: Controlling Z Position (depth) (current: " << shape2PosZ << ")" << std::endl;
            keyPressedS3 = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_RELEASE) keyPressedS3 = false;

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && !keyPressedS4) {
            currentMode2 = MODE_ROTATION;
            std::cout << "Shape 2: Controlling Rotation (current: " << shape2Rotation << ")" << std::endl;
            keyPressedS4 = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_RELEASE) keyPressedS4 = false;

        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && !keyPressedS5) {
            currentMode2 = MODE_SCALE;
            std::cout << "Shape 2: Controlling Scale (current: " << shape2Scale << ")" << std::endl;
            keyPressedS5 = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_5) == GLFW_RELEASE) keyPressedS5 = false;

        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS && !keyPressedS6) {
            currentMode2 = MODE_ALPHA;
            std::cout << "Shape 2: Controlling Transparency (current: " << shape2Alpha << ")" << std::endl;
            keyPressedS6 = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_6) == GLFW_RELEASE) keyPressedS6 = false;

        if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS && !keyPressedS7) {
            currentMode2 = MODE_COLOR_R;
            std::cout << "Shape 2: Controlling Red Color (current: " << shape2Color[0] << ")" << std::endl;
            keyPressedS7 = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_7) == GLFW_RELEASE) keyPressedS7 = false;

        if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS && !keyPressedS8) {
            currentMode2 = MODE_COLOR_G;
            std::cout << "Shape 2: Controlling Green Color (current: " << shape2Color[1] << ")" << std::endl;
            keyPressedS8 = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_RELEASE) keyPressedS8 = false;

        if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS && !keyPressedS9) {
            currentMode2 = MODE_COLOR_B;
            std::cout << "Shape 2: Controlling Blue Color (current: " << shape2Color[2] << ")" << std::endl;
            keyPressedS9 = true;
        }
        else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_RELEASE) keyPressedS9 = false;
    }

    // Adjust Shape 2 with WASD
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        switch (currentMode2) {
        case MODE_POSITION_X: shape2PosX += 0.01f; break;
        case MODE_POSITION_Y: shape2PosY += 0.01f; break;
        case MODE_POSITION_Z: shape2PosZ += 0.01f; break;
        case MODE_ROTATION: shape2Rotation += 0.02f; break;
        case MODE_SCALE: shape2Scale += 0.01f; break;
        case MODE_ALPHA: shape2Alpha = fmin(1.0f, shape2Alpha + 0.01f); break;
        case MODE_COLOR_R: shape2Color[0] = fmin(1.0f, shape2Color[0] + 0.01f); break;
        case MODE_COLOR_G: shape2Color[1] = fmin(1.0f, shape2Color[1] + 0.01f); break;
        case MODE_COLOR_B: shape2Color[2] = fmin(1.0f, shape2Color[2] + 0.01f); break;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        switch (currentMode2) {
        case MODE_POSITION_X: shape2PosX -= 0.01f; break;
        case MODE_POSITION_Y: shape2PosY -= 0.01f; break;
        case MODE_POSITION_Z: shape2PosZ -= 0.01f; break;
        case MODE_ROTATION: shape2Rotation -= 0.02f; break;
        case MODE_SCALE: shape2Scale = fmax(0.1f, shape2Scale - 0.01f); break;
        case MODE_ALPHA: shape2Alpha = fmax(0.0f, shape2Alpha - 0.01f); break;
        case MODE_COLOR_R: shape2Color[0] = fmax(0.0f, shape2Color[0] - 0.01f); break;
        case MODE_COLOR_G: shape2Color[1] = fmax(0.0f, shape2Color[1] - 0.01f); break;
        case MODE_COLOR_B: shape2Color[2] = fmax(0.0f, shape2Color[2] - 0.01f); break;
        }
    }

    // Reset Shape 1 (R key)
    static bool keyPressedR = false;
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS && !keyPressedR) {
        shape1PosX = -0.3f;
        shape1PosY = 0.0f;
        shape1PosZ = 0.0f;
        shape1Rotation = 0.0f;
        shape1Scale = 1.0f;
        shape1Alpha = 0.6f;
        shape1Color[0] = 1.0f;
        shape1Color[1] = 0.0f;
        shape1Color[2] = 0.0f;
        std::cout << "Shape 1 Reset to Default" << std::endl;
        keyPressedR = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE) keyPressedR = false;

    // Reset Shape 2 (T key)
    static bool keyPressedT = false;
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !keyPressedT) {
        shape2PosX = 0.3f;
        shape2PosY = 0.0f;
        shape2PosZ = 0.5f;
        shape2Rotation = 0.0f;
        shape2Scale = 1.0f;
        shape2Alpha = 1.0f;
        shape2Color[0] = 0.0f;
        shape2Color[1] = 0.0f;
        shape2Color[2] = 1.0f;
        std::cout << "Shape 2 Reset to Default" << std::endl;
        keyPressedT = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE) keyPressedT = false;

    // Toggle wireframe with Space
    static bool wireframeMode = false;
    static bool keyPressedSpace = false;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !keyPressedSpace) {
        wireframeMode = !wireframeMode;
        if (wireframeMode) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            std::cout << "Wireframe Mode ON" << std::endl;
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            std::cout << "Wireframe Mode OFF" << std::endl;
        }
        keyPressedSpace = true;
    }
    else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) keyPressedSpace = false;
}

int main()
{
    // --- Initialize GLFW ---
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "4 Buttons + Z-Axis + Transparency - FIXED", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // --- Initialize GLEW ---
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // ==========================================
    // Enable OpenGL features
    // ==========================================
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- Build shaders ---
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

    // ==========================================
    // Define vertices for BOTH triangles
    // ==========================================
    float vertices[] = {
        // Triangle 1 (Red base)
        -0.2f, -0.2f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.2f, -0.2f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.0f,  0.2f, 0.0f,  1.0f, 0.0f, 0.0f,

         // Triangle 2 (Blue base)
         -0.2f, -0.2f, 0.0f,  0.0f, 0.0f, 1.0f,
          0.2f, -0.2f, 0.0f,  0.0f, 0.0f, 1.0f,
          0.0f,  0.2f, 0.0f,  0.0f, 0.0f, 1.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Print instructions
    std::cout << "\n========== CONTROLS ==========" << std::endl;
    std::cout << "SHAPE 1 (Red Triangle):" << std::endl;
    std::cout << "  1: Position X | 2: Position Y | 3: Position Z" << std::endl;
    std::cout << "  4: Rotation | 5: Scale | 6: Transparency" << std::endl;
    std::cout << "  7: Red | 8: Green | 9: Blue" << std::endl;
    std::cout << "  Arrow Keys: Increase/Decrease" << std::endl;
    std::cout << "\nSHAPE 2 (Blue Triangle):" << std::endl;
    std::cout << "  Shift + 1-9 to select mode" << std::endl;
    std::cout << "  W/S: Increase/Decrease" << std::endl;
    std::cout << "\nR: Reset Shape 1 | T: Reset Shape 2" << std::endl;
    std::cout << "Space: Toggle Wireframe" << std::endl;
    std::cout << "==============================\n" << std::endl;

    // --- Render Loop ---
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Clear color and depth buffers
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        // Get uniform locations
        int transformLoc = glGetUniformLocation(shaderProgram, "transform");
        int alphaLoc = glGetUniformLocation(shaderProgram, "uAlpha");
        int colorLoc = glGetUniformLocation(shaderProgram, "uColor");

        // ==========================================
        // [FIX] Sort shapes by Z position (farther first)
        // ==========================================
        if (shape1PosZ > shape2PosZ) {
            // Shape 1 is farther, draw it first
            float transformMatrix1[16];
            createTransformMatrix(transformMatrix1,
                shape1PosX, shape1PosY, shape1PosZ,
                0.0f, 0.0f, shape1Rotation,
                shape1Scale);

            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transformMatrix1);
            glUniform1f(alphaLoc, shape1Alpha);
            glUniform3f(colorLoc, shape1Color[0], shape1Color[1], shape1Color[2]);

            glDrawArrays(GL_TRIANGLES, 0, 3);

            // Then draw Shape 2 (closer)
            float transformMatrix2[16];
            createTransformMatrix(transformMatrix2,
                shape2PosX, shape2PosY, shape2PosZ,
                0.0f, 0.0f, shape2Rotation,
                shape2Scale);

            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transformMatrix2);
            glUniform1f(alphaLoc, shape2Alpha);
            glUniform3f(colorLoc, shape2Color[0], shape2Color[1], shape2Color[2]);

            glDrawArrays(GL_TRIANGLES, 3, 3);
        }
        else {
            // Shape 2 is farther OR they have same Z, draw Shape 2 first
            float transformMatrix2[16];
            createTransformMatrix(transformMatrix2,
                shape2PosX, shape2PosY, shape2PosZ,
                0.0f, 0.0f, shape2Rotation,
                shape2Scale);

            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transformMatrix2);
            glUniform1f(alphaLoc, shape2Alpha);
            glUniform3f(colorLoc, shape2Color[0], shape2Color[1], shape2Color[2]);

            glDrawArrays(GL_TRIANGLES, 3, 3);

            // Then draw Shape 1 (closer)
            float transformMatrix1[16];
            createTransformMatrix(transformMatrix1,
                shape1PosX, shape1PosY, shape1PosZ,
                0.0f, 0.0f, shape1Rotation,
                shape1Scale);

            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transformMatrix1);
            glUniform1f(alphaLoc, shape1Alpha);
            glUniform3f(colorLoc, shape1Color[0], shape1Color[1], shape1Color[2]);

            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}