#include "common.h"
#include "Image.h"
#include "Player.h"
#include "unistd.h"

#define GLFW_DLL
#include <GLFW/glfw3.h>

constexpr GLsizei WINDOW_WIDTH = 640, WINDOW_HEIGHT = 640;
constexpr int size = 640;
struct InputState
{
  bool keys[1024]{}; //массив состояний кнопок - нажата/не нажата
  GLfloat lastX = 400, lastY = 300; //исходное положение мыши
  bool firstMouse = true;
  bool captureMouse         = true;  // Мышка захвачена нашим приложением или нет?
  bool capturedMouseJustNow = false;
} static Input;


GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;


void OnKeyboardPressed(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	switch (key)
	{
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
  case GLFW_KEY_1:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    break;
  case GLFW_KEY_2:
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    break;
	default:
		if (action == GLFW_PRESS)
      Input.keys[key] = true;
		else if (action == GLFW_RELEASE)
      Input.keys[key] = false;
	}
}

int processPlayerMovement(Image &screen, Player &player)
{
	int code = 0;
	if(Input.keys[GLFW_KEY_SPACE])
		player.ProcessAction(screen);
	if(Input.keys[GLFW_KEY_W])
		code = player.ProcessInput(MovementDir::UP);
	else if(Input.keys[GLFW_KEY_S])
			code = player.ProcessInput(MovementDir::DOWN);
	if(Input.keys[GLFW_KEY_A])
    	code = player.ProcessInput(MovementDir::LEFT);
	else if(Input.keys[GLFW_KEY_D])
			code = player.ProcessInput(MovementDir::RIGHT);
	return code;
}

void PrintMsg(Image &screen, const std::string &path, int width, int height)
{
	Pixel pix;
	Image img(path);
	for(int i = (size - width)/2; i < size - (size - width)/2; i++)
	{
		for(int j = (size - height)/2; j < size - (size - height)/2; j++)
		{
			pix = img.GetPixel(i - (size - width)/2, j - (size - height)/2);
			screen.PutPixel(i, size - j, pix);
		}
	}
}


void OnMouseButtonClicked(GLFWwindow* window, int button, int action, int mods)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    Input.captureMouse = !Input.captureMouse;

  if (Input.captureMouse)
  {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    Input.capturedMouseJustNow = true;
  }
  else
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

}

void OnMouseMove(GLFWwindow* window, double xpos, double ypos)
{
  if (Input.firstMouse)
  {
    Input.lastX = float(xpos);
    Input.lastY = float(ypos);
    Input.firstMouse = false;
  }

  GLfloat xoffset = float(xpos) - Input.lastX;
  GLfloat yoffset = Input.lastY - float(ypos);

  Input.lastX = float(xpos);
  Input.lastY = float(ypos);
}


void OnMouseScroll(GLFWwindow* window, double xoffset, double yoffset)
{
  // ...
}


int initGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	std::cout << "Controls: "<< std::endl;
	std::cout << "press right mouse button to capture/release mouse cursor  "<< std::endl;
	std::cout << "W, A, S, D - movement  "<< std::endl;
	std::cout << "SPACE - break wall  "<< std::endl;
	std::cout << "press ESC to exit" << std::endl;

	return 0;
}

void drawPixAsTile(Image &screen, Pixel &pix, int x, int y)
{
	for(int i = 0; i < tileSize; i++)
	{
		for(int j = 0; j < tileSize; j++)
		{
			screen.PutPixel(x + i, y + j, pix);
		}
	}
}

void DrawLevel(Image &screen, std::ifstream &file, Player &player)
{
	char ch;
	int i = 0;
	int j = lvlSize - 1;
	Point start;
	Pixel finish{0, 255, 0, 0};
	Image floor("./resources/floor.png");
	Image wall("./resources/wall.png");
	Image brwall("./resources/wall_break.png");
	Image empty("./resources/void.png");
	while(file.get(ch))
	{
		switch(ch)
		{
		case '.':
			floor.Draw(screen, i*tileSize, j*tileSize);
			player.FillMap(i ,j, 0);
			break;
		case 'x':
			drawPixAsTile(screen, finish, i*tileSize, j*tileSize);
			player.FillMap(i, j, 1);
			break;
		case ' ':
			empty.Draw(screen, i*tileSize, j*tileSize);
			player.FillMap(i, j, 2);
			break;
		case '#':
			wall.Draw(screen, i*tileSize, j*tileSize);
			player.FillMap(i, j, 3);
			break;
		case '%':
			brwall.Draw(screen, i*tileSize, j*tileSize);
			player.FillMap(i, j, 4);
			break;
		case '@':
			floor.Draw(screen, i*tileSize, j*tileSize);
			player.Init(i*tileSize, j*tileSize);
			player.FillMap(i, j, 0);
			break;
		case '\n':
			i = -1;
			j--;
			break;
		}
		i++;
	}
}

void CreateLevel(const std::string &path, Image &screen, Player &player)
{
	std::ifstream lvl;
	lvl.open(path);
	DrawLevel(screen, lvl, player);
	lvl.close();
}

int main(int argc, char** argv)
{
	int lvl_counter = 0;
	int state = 0;
	bool game_over = false;
	bool lvl_up = false;
	if(!glfwInit())
    return -1;

//	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow*  window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Sergey Gorbunov 316", nullptr, nullptr);
	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback        (window, OnKeyboardPressed);
	glfwSetCursorPosCallback  (window, OnMouseMove);
	glfwSetMouseButtonCallback(window, OnMouseButtonClicked);
	glfwSetScrollCallback     (window, OnMouseScroll);

	if(initGL() != 0)
		return -1;

  //Reset any OpenGL errors which could be present for some reason
	GLenum gl_error = glGetError();
	while (gl_error != GL_NO_ERROR)
		gl_error = glGetError();

	Player player;
	Image screenBuffer(WINDOW_WIDTH, WINDOW_HEIGHT, 4);
	CreateLevel("./resources/lvl1.txt", screenBuffer, player);
	player.ExtractBackground(screenBuffer);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);  GL_CHECK_ERRORS;
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f); GL_CHECK_ERRORS;

  //game loop
	while(!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
    glfwPollEvents();
	if(!game_over)
    	state = processPlayerMovement(screenBuffer, player);
	switch(state)
	{
	case 0:
		break;
	case 1:
		if(lvl_counter == 0)
		{
			lvl_counter++;
			lvl_up = true;
			PrintMsg(screenBuffer, "./resources/level_up.png", 574, 530);
		}else{
			PrintMsg(screenBuffer, "./resources/you_win.png", 500, 200);
			game_over = true;
		}
		break;
	case 2:
		if(!game_over)
			PrintMsg(screenBuffer, "./resources/you_lose.png", 500, 200);
		game_over = true;
	case 3:
		break;
	}
	if(!game_over && !lvl_up)
    	player.Draw(screenBuffer);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); GL_CHECK_ERRORS;
    glDrawPixels (WINDOW_WIDTH, WINDOW_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, screenBuffer.Data()); GL_CHECK_ERRORS;

		glfwSwapBuffers(window);
		if(lvl_up){
			sleep(1.5);
			CreateLevel("./resources/lvl2.txt", screenBuffer, player);
			lvl_up = false;
		}
	}

	glfwTerminate();
	return 0;
}
