#include "application.h"

GLFWwindow* window = NULL;

int InitWindowFailed() {
	if(glfwInit() == GLFW_FAIL){
		fprintf(stderr, "failed to initialize GLFW\n");
		return EXIT_WITH_ERROR;
	}

	glfwWindowHint(GLFW_SAMPLES, ANTIALIASING);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPEN_GL_VERSION);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPEN_GL_VERSION);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  //Gets working on MAC
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // we don't want old open GL

	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, APP_NAME, NULL, NULL);

	if(window == NULL) {
		fprintf(stderr, "failed to create/open GLFW window.\n");
		glfwTerminate();
		return EXIT_WITH_ERROR;
	}
	
	glfwMakeContextCurrent(window);

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
		
	return EXIT_WITH_SUCCESS;
}

int InitGlewFailed(){
	glewExperimental = true;   //Has to do with core profile
	int init = (int)glewInit();
	if(glewInit() != GLFW_OK) {
		fprintf(stderr, "failed to initialize GLEW\n");
		return EXIT_WITH_ERROR;
	}

	return EXIT_WITH_SUCCESS;
}

GLuint LoadShaders(const char *vertex_file_path, const char *fragment_file_path) {
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	string vertexShaderCode = "";
	ifstream vertexShaderStream(vertex_file_path, ios::in);
	if(vertexShaderStream.is_open()){
		string line = "";
		while(getline(vertexShaderStream, line)){
			vertexShaderCode += "\n" + line;
		}
		vertexShaderStream.close();
	}
	
	string fragmentShaderCode = "";
	ifstream fragmentShaderStream(fragment_file_path, ios::in);
	if(fragmentShaderStream.is_open()){
		string line = "";
		while(getline(fragmentShaderStream, line)){
			fragmentShaderCode += "\n" + line;
		}
		fragmentShaderStream.close();
	}

	GLint result = GL_FALSE;
	int infoLogLength = 0;

	// Compile Shaders Here
	printf("Compiling Vertex Shader: %\n", vertex_file_path);
	char const *vertexSourcePointer = vertexShaderCode.c_str();
	glShaderSource(vertexShaderID, 1, &vertexSourcePointer, NULL);
	glCompileShader(vertexShaderID);

	//Check Vertex Shader...
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

	char* vertexShaderErrorMessage = new char[infoLogLength];
	glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, &vertexShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &vertexShaderErrorMessage[0]);

	//Compile Fragment Shader...
	printf("Compiling Fragment Shader: %\n", fragment_file_path);
	char const *fragmentSourcePointer = fragmentShaderCode.c_str();
	glShaderSource(fragmentShaderID, 1, &fragmentSourcePointer, NULL);
	glCompileShader(fragmentShaderID);

	//Check Vertex Shader...
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);

	char* fragmentShaderErrorMessage = new char[infoLogLength];
	glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, &fragmentShaderErrorMessage[0]);
	fprintf(stdout, "%s\n", &fragmentShaderErrorMessage[0]);
	
	//Link Program...
	fprintf(stdout, "Linking Program\n");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glLinkProgram(programID);

	//Check the program...
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	char *programErrorMessage = new char[glm::max(infoLogLength, int(1))];
	glGetProgramInfoLog(programID, infoLogLength, NULL, &programErrorMessage[0]);
	fprintf(stdout, "%s\n", &programErrorMessage[0]);

	glDeleteShader(vertexShaderID);
	glDeleteShader(fragmentShaderID);

	delete(programErrorMessage);
	programErrorMessage = NULL;

	delete(vertexShaderErrorMessage);
	vertexShaderErrorMessage = NULL;

	delete(fragmentShaderErrorMessage);
	fragmentShaderErrorMessage = NULL;

	return programID;

}


GLuint& LoadTriangle() {
	static const GLfloat g_vertex_buffer_data[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	GLuint vertexBuffer = 0;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	return vertexBuffer;
}

void RenderTriangle(GLuint vertexBuffer) {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

	glVertexAttribPointer(
		0,				//attribute layout...
		3,				//elements existing in array...
		GL_FLOAT,		//each element is a type float...
		GL_FALSE,		//Noramlized?...
		0,				//Stride...
		(void*)0		//Array Buffer offset...
		);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	glDisableVertexAttribArray(0);
}

int main () {
	if(InitWindowFailed() | InitGlewFailed()){
		return EXIT_WITH_ERROR;
	}

	GLuint vertexArrayID = 0;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	//Create and compile glsl program from shaders...
	GLuint programID = LoadShaders("BasicVertexShader.vertexShader", "BasicFragmentShader.fragmentshader");

	GLuint triangleID = LoadTriangle();

	

	do{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);

		RenderTriangle(triangleID);
		//update();
		//render();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	return EXIT_WITH_SUCCESS;


}
