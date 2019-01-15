/* Lab 6 base code - transforms using local matrix functions
	to be written by students -
	based on lab 5 by CPE 471 Cal Poly Z. Wood + S. Sueda
	& Ian Dunn, Christian Eckhardt
*/
#include <iostream>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "GLSL.h"
#include "Program.h"
#include "Shape.h"
#include "MatrixStack.h"
#include "WindowManager.h"
#include "camera.h"
#include "Audio.h"
// used for helper in perspective
//#include <glm/glm.hpp>
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <random>
using namespace std;
using namespace glm;

#include "Accelerate/accelerate.h"

#include <list>

using std::list;

/**** OpenAL setup ****/
#define FREQ 22050   // Sample rate
#define CAP_SIZE 2048 // How much to capture at a time (affects latency)

short buffer[FREQ*2]; // A buffer to hold captured audio
ALubyte texels[100*100*4];
ALubyte amplitudes2[1024];
float amplitudes[1024];
float buf2[2048];
float curVal = 1.0f;
float oldVal = 1.0f;
float addOn = 0.02f;
float shift = 0.00005f;
float curShift = 0.0f;
float decrement = 0.00005f;

//Random Number Set Up
const int range_from  = 0;
int range_to;
std::random_device                  rand_dev;
std::mt19937                        generator(rand_dev());


class Application : public EventCallbacks
{

public:

	WindowManager * windowManager = nullptr;
    
    //struct for audio data
    typedef struct
    {
        float Bass;
        float LowMid;
        float Mid;
        float HighMid;
        float High;
    }AudioData;

	// Our shader program
	std::shared_ptr<Program> progB;
    std::shared_ptr<Program> progO;

	// Shape to be used (from obj file)
	shared_ptr<Shape> shape1, shape2;
	
	//camera
	camera mycam;

	//texture for sim
	GLuint Texture;
	GLuint Texture2;

	// Contains vertex information for OpenGL
	GLuint VertexArrayID,vao_effect;

	// Data necessary to give our triangle to OpenGL
	GLuint VertexBufferID, vbo_effect, vbo_effect2;
	GLuint GeoShader;
    
    int maxBuffSize;
    int maxVerts;
    vector<float> smallerBuf = {0};
    vector<float> biggerBuf = {0};
    
    Audio audio;
    AudioData audioData = {0};

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			curVal += 0.1f;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			mycam.w = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 1;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			mycam.a = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			mycam.a = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			mycam.d = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			mycam.d = 0;
		}
	}

	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{
		double posX, posY;

		if (action == GLFW_PRESS)
		{
			glfwGetCursorPos(window, &posX, &posY);
			cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
		}
	}

	void resizeCallback(GLFWwindow *window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	void init(const std::string& resourceDirectory)
	{


		GLSL::checkVersion();

		
		// Set background color.
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		//culling:
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);

		//transparency
		glEnable(GL_BLEND);
		//next function defines how to mix the background color with the transparent pixel in the foreground. 
		//This is the standard:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 

		// Initialize the GLSL program.
		progB = make_shared<Program>();
		progB->setVerbose(true);
		progB->setShaderNames(resourceDirectory + "/billboardVert.glsl",
			resourceDirectory + "/billboardFrag.glsl",
			resourceDirectory + "/billboardGeom.glsl");
        if (! progB->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
        progB->init();
        progB->addUniform("P");
        progB->addUniform("V");
		progB->addUniform("VRi");
		progB->addUniform("M");
        progB->addUniform("bScale");
        progB->addUniform("morphT");
        progB->addAttribute("vertPos");


        progO = make_shared<Program>();
        progO->setVerbose(true);
        progO->setShaderNames(resourceDirectory + "/objectVert.glsl",
                              resourceDirectory + "/objectFrag.glsl",
                              "");
        if (! progO->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
            exit(1);
        }
        progO->init();
        progO->addUniform("P");
        progO->addUniform("MV");
        progO->addAttribute("vertPos");
        progO->addAttribute("vertNor");

	}

	void initGeom(const std::string& resourceDirectory)
	{
        //NOTE: Different objs can be used but it may change orientation depending on which one is bigger
        //NEED TO FIX: Different objs may not always be centered facing the camera
        //FUTURE UPDATE: Enable the use of more than 2 objs and swap in on the unused vertPos in Vertex Shader
        // Initialize face mesh.
		shape1 = make_shared<Shape>();
		shape1->loadMesh(resourceDirectory + "/womanface.obj");
		shape1->resize();
		shape1->init();
        
        //Initialize skull mesh.
        shape2 = make_shared<Shape>();
        shape2->loadMesh(resourceDirectory + "/Skull.obj");
        shape2->resize();
        shape2->init();
		
        //Initialize Audio FFT
		int width, height, channels;
		char filepath[1000];

		//texture 1
		string str = resourceDirectory + "/beamspot.png";
		strcpy(filepath, str.c_str());		
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		//texture 2
		str = resourceDirectory + "/greenpixel2.jpg";
		strcpy(filepath, str.c_str());
		data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture2);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, Texture2);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
        
        

		//[TWOTEXTURES]
		//set the 2 textures to the correct samplers in the fragment shader:
        GLuint Tex1Location = glGetUniformLocation(progB->pid, "tex");//tex, tex2... sampler in the fragment shader
        GLuint Tex2Location = glGetUniformLocation(progB->pid, "tex2");
		// Then bind the uniform samplers to texture units:
        glUseProgram(progB->pid);
		glUniform1i(Tex1Location, 0);
		glUniform1i(Tex2Location, 1);


		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		GLfloat g_vertex_buffer_data[] = 
		{
			0,0,0,
			1,0,0,
			1,1,0,
			2,0,0,
		};
		//make it a bit smaller
		for (int i = 0; i < sizeof(g_vertex_buffer_data) / sizeof(float); i++)
			g_vertex_buffer_data[i] *= 0.5;
		//actually memcopy the data - only do this once
		glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glBindVertexArray(0);

        if (shape1->posBuf.size() > shape2->posBuf.size())
        {
            //cout << "buffer 1 is bigger" << endl;
            //Maximum buffer size of the larger of the two buffers
            maxBuffSize = shape1->posBuf.size();
            //Maximum number of verts that can come from the max buffer size
            maxVerts = maxBuffSize/3;
            //Max extent of the smaller buffer size for iteration/indexing purposes
            range_to = shape2->posBuf.size() / 3;
            smallerBuf = shape2->posBuf;
            biggerBuf = shape1->posBuf;
            while (smallerBuf.size() / 3 < maxVerts)
            {
                std::uniform_int_distribution<int>  distr(range_from, range_to);
                int temp = distr(generator);
                //cout << "this is a temp random number for iteration/indexing: " << temp << endl;
                smallerBuf.push_back(shape1->posBuf.at(temp*3+0));
                smallerBuf.push_back(shape1->posBuf.at(temp*3+1));
                smallerBuf.push_back(shape1->posBuf.at(temp*3+2));
            }
            //biggerBuf(shape2->posBuf, &newBuf);
        }
        else
        {
            //cout << "buffer 2 is bigger" << endl
            maxBuffSize = shape2->posBuf.size();
            maxVerts = maxBuffSize/3;
            range_to = shape1->posBuf.size() / 3;
            smallerBuf = shape1->posBuf;
            biggerBuf = shape2->posBuf;
            while (smallerBuf.size() / 3 < maxVerts)
            {
                std::uniform_int_distribution<int>  distr(range_from, range_to);
                int temp = distr(generator);
                //cout << "this is a temp random number for iteration/indexing: " << temp << endl;
                smallerBuf.push_back(shape1->posBuf.at(temp*3+0));
                smallerBuf.push_back(shape1->posBuf.at(temp*3+1));
                smallerBuf.push_back(shape1->posBuf.at(temp*3+2));
            }
            //biggerBuf(shape1->posBuf, &newBuf);
        }
        //Initialize data for obj1 morph
		//generate the VAO
		glGenVertexArrays(1, &vao_effect);
		glBindVertexArray(vao_effect);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &vbo_effect);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, vbo_effect);
		//actually memcopy the data - only do this once
		//Working
        //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * shape1->posBuf.size(), shape1->posBuf.data(), GL_DYNAMIC_DRAW);
        //Test
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * smallerBuf.size(), smallerBuf.data(), GL_DYNAMIC_DRAW);
		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
        
        //Initialize data for obj2 morph
        //generate vertex buffer to hand off to OGL
        glGenBuffers(1, &vbo_effect2);
        //set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, vbo_effect2);
        //actually memcopy the data - only do this once
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * biggerBuf.size(), biggerBuf.data(), GL_DYNAMIC_DRAW);
        //we need to set up the vertex array
        glEnableVertexAttribArray(1);
        //key function to get up how many elements to pull out at a time (3)
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindVertexArray(0);
	}

	void render()
	{
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width / (float)height;
		glViewport(0, 0, width, height);

		auto P = std::make_shared<MatrixStack>();
		auto MV = std::make_shared<MatrixStack>();
		P->pushMatrix();	
		P->perspective(70., width, height, 0.1, 100.0f);
		glm::mat4 mv ,T, Tshift, R;

		

		MV->pushMatrix();

		MV->translate(glm::vec3(0,0,-4.5));

		mv = MV->topMatrix();
		glm:mat4 V = mycam.process();
		

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		// Draw mesh using GLSL
        progB->bind();
        glUniformMatrix4fv(progB->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));

		T = glm::translate(glm::mat4(1), glm::vec3(0, 0, -10));
        glm::mat4 S, initS;
		
        static float g = 0.0;
		//float t = sin(g) + 0.5f;
		g += 0.15;
        //float angle = 135.0f;
        //glm::vec3 axis = glm::vec3(1.0f, 0.0f, 0.0f);
        //R = rotate(glm::mat4(1.0), angle, axis);
        
        initS = glm::scale(glm::mat4(1), glm::vec3(4, 4, 4));
        bool detected = false;
        if (curVal > 2.50)
        {
            curVal = 0;
        }
        for (int i = 125; i < 175; i++)
        {
            if ((int)amplitudes2[i] > 250   && detected == false)
            {
                S = glm::scale(glm::mat4(1), glm::vec3(curVal + addOn, curVal + addOn, curVal + addOn));
                //Tshift = glm::translate(glm::mat4(1), glm::vec3(0, 0, curShift));
                //curShift += shift*5;
                detected = true;
                oldVal = curVal;
                curVal += addOn;
            }
            else if ((int)amplitudes2[i] > 275  && detected == false)
            {
                S = glm::scale(glm::mat4(1), glm::vec3(curVal + (addOn*2), curVal + (addOn*2), curVal + (addOn*2)));
                //Tshift = glm::translate(glm::mat4(1), glm::vec3(0, 0, curShift));
                //curShift += shift*4;
                detected = true;
                oldVal = curVal;
                curVal += addOn*2;
            }
            else if ((int)amplitudes2[i] > 300  && detected == false)
            {
                S = glm::scale(glm::mat4(1), glm::vec3(curVal + (addOn*4), curVal + (addOn*4), curVal + (addOn*4)));
                //Tshift = glm::translate(glm::mat4(1), glm::vec3(0, 0, curShift));
                //curShift += shift;
                detected = true;
                oldVal = curVal;
                curVal += addOn * 4;
            }
            else
            {
                if (curVal < 0)
                {
                    curVal = 0;
                }
                S = glm::scale(glm::mat4(1), glm::vec3(curVal - decrement, curVal - decrement, curVal - decrement));
                //Tshift = glm::translate(glm::mat4(1), glm::vec3(0, 0, curShift));
                //curShift -= shift*2;
                curVal -= decrement * 3.5;
            }
        }
		mv = T*R*S*initS;
		
		glm::mat4 VRi = V;
		VRi[3][0] = 0;
		VRi[3][1] = 0;
		VRi[3][2] = 0;
		VRi[0][3] = 0;
		VRi[1][3] = 0;
		VRi[2][3] = 0;
		VRi = glm::transpose(VRi);
        //Scale for the Billboard Size in the Geometry Shader
        glUniform1f(progB->getUniform("bScale"), curVal);
        glUniform1f(progB->getUniform("morphT"), curVal);
		glUniformMatrix4fv(progB->getUniform("VRi"), 1, GL_FALSE, &VRi[0][0]);
		glUniformMatrix4fv(progB->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(progB->getUniform("M"), 1, GL_FALSE, &mv[0][0]);
		
		glBindVertexArray(vao_effect);
		//glBindVertexArray(VertexArrayID);
		//***WORKING LINE BELOW
        //int tempSize = shape1->posBuf.size()/3;
        //cout << "smaller: " << smallerBuf.size() << "bigger: " << biggerBuf.size()<< endl;
 		glDisable(GL_DEPTH_TEST);
		glDrawArrays(GL_POINTS, 0, maxVerts);
		glEnable(GL_DEPTH_TEST);
        progB->unbind();
        
        
        /*
        progO->bind();
        glUniformMatrix4fv(progO->getUniform("P"), 1, GL_FALSE, glm::value_ptr(P->topMatrix()));
        glUniformMatrix4fv(progO->getUniform("MV"), 1, GL_FALSE, &mv[0][0]);
        shape->draw(progO);
        progO->unbind();
         */
	}
    
    void fft(short buffer[])
    {
        //******* set up fourier transform *******//
        //clear texel buffer
        for (int i=0; i<40000; i++) {
            texels[i] = 0;
        }
        
        //create float array from short array
        copy(buffer, buffer + 2048, buf2);
        
        const int n = 2048;
        const int log2n = 11; // 2^11 = 2048
        
        DSPSplitComplex a;
        a.realp = new float[n/2];
        a.imagp = new float[n/2];
        
        // prepare the fft algo (you want to reuse the setup across fft calculations)
        FFTSetup setup = vDSP_create_fftsetup(log2n, kFFTRadix2);
        
        // copy the input to the packed complex array that the fft algo uses
        vDSP_ctoz((DSPComplex *) buf2, 2, &a, 1, n/2);
        
        // calculate the fft
        vDSP_fft_zrip(setup, &a, 1, log2n, FFT_FORWARD);
        
        // do something with the complex spectrum
        int k=0;
        for (size_t i = 0; i < n/2; i++) {
            amplitudes[k] = a.imagp[i];
            k++;
        }
        
        //map each value from 'float' array to size of byte (originally size of short)
        for (int i=0; i<1024; i++) {
            amplitudes2[i] = (amplitudes[i] - SHRT_MIN)/(SHRT_MAX - SHRT_MIN) * 255;
        }
    }
   
    /*
    void biggerBuf(vector<float> oldBuf, vector<float> *newBuf)
    {
        *newBuf = oldBuf;
        newBuf.resize(maxBuffSize);
    }
  */
};

//*********************************************************************************************************
int main(int argc, char **argv)
{
	// Where the resources are loaded from
	std::string resourceDir = "../../resources";

	if (argc >= 2)
	{
		resourceDir = argv[1];
	}


	Application *application = new Application();

    // Inititalize audio
    Audio *audio = new Audio();
    audio->initAL();
    
	// Your main will always include a similar set up to establish your window
	// and GL context, etc.

	WindowManager *windowManager = new WindowManager();
	windowManager->init(640, 480);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	// This is the code that will likely change program to program as you
	// may need to initialize or set up different data and state

	application->init(resourceDir);
	application->initGeom(resourceDir);

	// Loop until the user closes the window.
	while (! glfwWindowShouldClose(windowManager->getHandle()))
	{
        audio->readAudio();
        
        // Render scene.
        application->fft(audio->buffer);
        /*
        cout << "amp slot 1: " << amplitudes[1] << endl;
        cout << "amp slot 50: " << amplitudes[50] << endl;
        cout << "amp slot 100: " << amplitudes[100] << endl;
        cout << "amp slot 150: " << amplitudes2[150] << endl;
        cout << "amp slot 200: " << amplitudes2[200] << endl;
        cout << "amp slot 250: " << amplitudes2[250] << endl;
        cout << "amp slot 300: " << amplitudes2[300] << endl;
        cout << "amp slot 350: " << amplitudes2[350] << endl;
        cout << "amp slot 400: " << amplitudes2[400] << endl;
        cout << "amp slot 450: " << amplitudes2[450] << endl;
        cout << "amp slot 500: " << amplitudes2[500] << endl;
        cout << "amp slot 550: " << amplitudes2[550] << endl;
        cout << "amp slot 600: " << amplitudes2[600] << endl;
        cout << "amp slot 650: " << amplitudes2[650] << endl;
        cout << "amp slot 700: " << amplitudes2[700] << endl;
        cout << "amp slot 750: " << amplitudes2[750] << endl;
        cout << "amp slot 800: " << amplitudes2[800] << endl;
        cout << "amp slot 850: " << amplitudes2[850] << endl;
        cout << "amp slot 900: " << amplitudes2[900] << endl;
        cout << "amp slot 950: " << amplitudes2[950] << endl;
        cout << "amp slot 1000: " << amplitudes2[1000] << endl;
        */
 		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
    audio->cleanAL();
	windowManager->shutdown();
	return 0;
}
