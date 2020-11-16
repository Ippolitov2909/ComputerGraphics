#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <FreeImage.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement(void);
void mouse_callback(GLFWwindow* window, double xpox, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


bool keys[1024];
const GLuint WIDTH = 800, HEIGHT = 600;
GLfloat deltatime = 0.0f;
GLfloat lastframe = 0.0f;
GLfloat lastX = 400;
GLfloat lastY = 300;
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;
GLfloat fov = 45.0f;
bool firstMouse = true;

const GLchar* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"layout (location = 1) in vec3 normal;\n"
"uniform mat4 proj;\n"
"uniform mat4 view;\n"
"uniform mat4 model;\n"
"out vec3 Normal;\n"
"out vec3 fragPos;\n"
"void main()\n"
"{\n"
"gl_Position = proj * view * model * vec4(position, 1.0f);\n"
"Normal = mat3(transpose(inverse(model))) * normal;\n"
"fragPos = vec3(model * vec4(position, 1.0));\n"
"}\0";

const GLchar* fragmentShaderSource = "#version 330 core\n"
"in vec3 Normal;\n"
"in vec3 fragPos;\n"
"out vec4 color;\n"
"uniform vec3 objectColor;\n"
"uniform vec3 lightColor;\n"
"uniform vec3 lightPos;\n"
"uniform vec3 viewPos;\n"
"void main()\n"
"{\n"
"float ambientStrength = 0.1;\n"
"vec3 ambient = ambientStrength * lightColor;\n"
"vec3 norm = normalize(Normal);\n"
"vec3 lightDir = normalize(lightPos - fragPos);\n"
"float diff = max(dot(norm, lightDir), 0.0);\n"
"vec3 diffuse = diff * lightColor;\n"
"float specularStrength = 0.7f;\n"
"vec3 viewDir = normalize(viewPos - fragPos);\n"
"vec3 reflectDir = reflect(-lightDir, norm);\n"
"float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);\n"
"vec3 specular = specularStrength * spec * lightColor;\n"
"vec3 res = (ambient + diffuse + specular) * objectColor;\n"
//"vec3 res = (specular) * objectColor;\n"
"color = vec4(res, 1.0);\n"
"}\0";

const GLchar* lightvertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 position;\n"
"uniform mat4 proj;\n"
"uniform mat4 view;\n"
"uniform mat4 model;\n"
"void main()\n"
"{\n"
"gl_Position = proj * view * model * vec4(position, 1.0);\n"
"}\n";

const GLchar* lightfragmentShaderSource = "#version 330 core\n"
"out vec4 color;\n"
"uniform vec3 lightColor;\n"
"void main()\n"
"{\n"
"color = vec4(lightColor, 1.0);\n"
"}\n";

glm::vec3 camerapos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 camerafront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraup = glm::vec3(0.0f, 1.0f, 0.0f);

int main()
{
	std::cout << "Starting GLFW context, OpenGL 3.3" << std::endl;
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scroll_callback);

	glewExperimental = GL_TRUE;
	glewInit();

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	GLuint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	//
	GLuint lightvertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(lightvertexShader, 1, &lightvertexShaderSource, NULL);
	glCompileShader(lightvertexShader);

	glGetShaderiv(lightvertexShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(lightvertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	GLuint lightfragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(lightfragmentShader, 1, &lightfragmentShaderSource, NULL);
	glCompileShader(lightfragmentShader);
	glGetShaderiv(lightfragmentShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(lightfragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	GLuint lightshaderProgram = glCreateProgram();
	glAttachShader(lightshaderProgram, lightvertexShader);
	glAttachShader(lightshaderProgram, lightfragmentShader);
	glLinkProgram(lightshaderProgram);
	glGetProgramiv(lightshaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(lightshaderProgram, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}
	glDeleteShader(lightvertexShader);
	glDeleteShader(lightfragmentShader);

	/*FREE_IMAGE_FORMAT format = FreeImage_GetFileType("container.jpg", 0);
	FIBITMAP* image = FreeImage_Load(format, "container.jpg");
	FIBITMAP* temp = image;
	image = FreeImage_ConvertTo32Bits(image);
	int w = FreeImage_GetWidth(image);
	int h = FreeImage_GetHeight(image);
	GLubyte* bits = (GLubyte*)FreeImage_GetBits(image);

	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)bits);
	glGenerateMipmap(GL_TEXTURE_2D);
	FreeImage_Unload(image);
	glBindTexture(GL_TEXTURE_2D, 0);*/

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	glm::vec3 positions[] = {
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(2.0f, 5.0f, -15.0f),
		glm::vec3(-5.0, -3.0, -12.0f)
	};


	GLuint VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	GLuint lightVAO;
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	glm::vec3 lightPos(1.0f, 1.0f, 0.8f);


	glEnable(GL_DEPTH_TEST);
	//game loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		GLfloat currentframe = glfwGetTime();
		deltatime = currentframe - lastframe;
		lastframe = currentframe;
		do_movement();
		glClearColor(0.0f, 0.05f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glBindTexture(GL_TEXTURE_2D, texture);

		glUseProgram(shaderProgram);
		//glm::mat4 model = glm::rotate(glm::mat4(1), (GLfloat)glfwGetTime(), glm::vec3(0.5f, 1.0f, 0.0f));
		glm::mat4 model = glm::mat4(1);
		glm::mat4 view = glm::lookAt(camerapos, camerapos + camerafront, cameraup);
		glm::mat4 proj = glm::perspective(glm::radians(fov), ((GLfloat)WIDTH) / (GLfloat)HEIGHT, 0.1f, 100.0f);
		GLuint modelloc = glGetUniformLocation(shaderProgram, "model");
		GLuint viewloc = glGetUniformLocation(shaderProgram, "view");
		GLuint projloc = glGetUniformLocation(shaderProgram, "proj");
		GLuint objectColorLoc = glGetUniformLocation(shaderProgram, "objectColor");
		GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
		GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
		GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");
		glUniform3f(viewPosLoc, camerapos.x, camerapos.y, camerapos.z);
		glUniform3f(objectColorLoc, 1.0f, 0.2f, 0.2f);
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
		glUniformMatrix4fv(viewloc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projloc, 1, GL_FALSE, glm::value_ptr(proj));
		glBindVertexArray(VAO);
		for (GLuint i = 0; i < 3; i++) {
			glm::mat4 tmpmodel = glm::translate(glm::mat4(1), positions[i]);
			tmpmodel = tmpmodel * model;
			glUniformMatrix4fv(modelloc, 1, GL_FALSE, glm::value_ptr(tmpmodel));

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}
		glBindVertexArray(0);

		glUseProgram(lightshaderProgram);

		model = glm::mat4(1);
		glm::mat4 tmpmodel = glm::rotate(glm::mat4(1), 0.001f * glm::radians((GLfloat)glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
		lightPos = glm::vec3(tmpmodel * glm::vec4(lightPos.x, lightPos.y, lightPos.z, 1.0f));
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lightColorLoc = glGetUniformLocation(lightshaderProgram, "lightColor");
		modelloc = glGetUniformLocation(shaderProgram, "model");
		viewloc = glGetUniformLocation(shaderProgram, "view");
		projloc = glGetUniformLocation(shaderProgram, "proj");
		glUniformMatrix4fv(modelloc, 1, GL_FALSE,glm::value_ptr(model));
		glUniformMatrix4fv(viewloc, 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projloc, 1, GL_FALSE, glm::value_ptr(proj));
		glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	GLfloat cameraspeed = 0.05f;
	if (action == GLFW_PRESS)
		keys[key] = true;
	else if (action == GLFW_RELEASE)
		keys[key] = false;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	/*if (key == GLFW_KEY_W)
		camerapos += cameraspeed * camerafront;
	if (key == GLFW_KEY_S)
		camerapos -= cameraspeed * camerafront;
	if (key == GLFW_KEY_A)
		camerapos -= cameraspeed * glm::normalize(glm::cross(camerafront, cameraup));
	if (key == GLFW_KEY_D)
		camerapos += cameraspeed * glm::normalize(glm::cross(camerafront, cameraup));*/
}
void do_movement()
{
	GLfloat cameraspeed = deltatime * 2.5f;
	if (keys[GLFW_KEY_W])
		camerapos += cameraspeed * camerafront;
	if (keys[GLFW_KEY_S])
		camerapos -= cameraspeed * camerafront;
	if (keys[GLFW_KEY_A])
		camerapos -= cameraspeed * glm::normalize(glm::cross(camerafront, cameraup));
	if (keys[GLFW_KEY_D])
		camerapos += cameraspeed * glm::normalize(glm::cross(camerafront, cameraup));
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = ypos - lastY;
	lastX = xpos;
	lastY = ypos;
	GLfloat sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f) { pitch = 89.0f; }
	if (pitch < -89.0f) { pitch = -89.0f; }

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	camerafront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}