#include "SDLGraphicsProgram.hpp"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

// Initialization function
// Returns a true or false value based on successful completion of setup.
// Takes in dimensions of window.
SDLGraphicsProgram::SDLGraphicsProgram(int w, int h):m_screenWidth(w),m_screenHeight(h){
	// Initialization flag
	bool success = true;
	// String to hold any errors that occur.
	std::stringstream errorStream;
	// The window we'll be rendering to
	m_window = NULL;
	// Render flag

	// Initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO)< 0){
		errorStream << "SDL could not initialize! SDL Error: " << SDL_GetError() << "\n";
		success = false;
	}
	else{
		//Use OpenGL 3.3 core
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
		SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
		// We want to request a double buffer for smooth updating.
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		//Create window
		m_window = SDL_CreateWindow( "Lab",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                m_screenWidth,
                                m_screenHeight,
                                SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN );

		// Check if Window did not create.
		if( m_window == NULL ){
			errorStream << "Window could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		//Create an OpenGL Graphics Context
		m_openGLContext = SDL_GL_CreateContext( m_window );
		if( m_openGLContext == NULL){
			errorStream << "OpenGL context could not be created! SDL Error: " << SDL_GetError() << "\n";
			success = false;
		}

		// Initialize GLAD Library
		if(!gladLoadGLLoader(SDL_GL_GetProcAddress)){
			errorStream << "Failed to iniitalize GLAD\n";
			success = false;
		}

		//Initialize OpenGL
		if(!InitGL()){
			errorStream << "Unable to initialize OpenGL!\n";
			success = false;
		}
  	}

    // If initialization did not work, then print out a list of errors in the constructor.
    if(!success){
        errorStream << "SDLGraphicsProgram::SDLGraphicsProgram - Failed to initialize!\n";
        std::string errors=errorStream.str();
        SDL_Log("%s\n",errors.c_str());
    }else{
        SDL_Log("SDLGraphicsProgram::SDLGraphicsProgram - No SDL, GLAD, or OpenGL, errors detected during initialization\n\n");
    }

	// SDL_LogSetAllPriority(SDL_LOG_PRIORITY_WARN); // Uncomment to enable extra debug support!
	GetOpenGLVersionInfo();

	lastDecimate = 0;

	std:: cout << "Press number keys to decimate." << "\n";
	std:: cout << "Press Space to switch to a new model." << "\n";
	std:: cout << "Press w to toggle between fill mode and wireframe mode." << "\n";
	std:: cout << "Press q to quit." << "\n";

    // Setup our objects
    m_object1 = new Object;
	m_object1->MakeGeometry("./../../common/objects/bunny_centered.obj");

	m_object2 = new Object;

	m_object3 = new Object;

	m_object4 = new Object;

	m_object = m_object1;

	wireframeOn = true;

	currentObjectnum = 0;

	decimateby = 0.0;
}


// Proper shutdown of SDL and destroy initialized objects
SDLGraphicsProgram::~SDLGraphicsProgram(){
    // Reclaim all of our objects
    delete m_object1;
	delete m_object2;
	delete m_object3;
	delete m_object4;

    //Destroy window
	SDL_DestroyWindow( m_window );
	// Point m_window to NULL to ensure it points to nothing.
	m_window = NULL;
	//Quit SDL subsystems
	SDL_Quit();
}


// Initialize OpenGL
// Setup any of our shaders here.
bool SDLGraphicsProgram::InitGL(){
	//Success flag
	bool success = true;

    // Setup our OpenGL State machine
    // TODO: Read this
    // The below command is new!
    // What we are doing, is telling opengl to create a depth(or Z-buffer) 
    // for us that is stored every frame.
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D); 
	return success;
}


// Update OpenGL
void SDLGraphicsProgram::Update(){

    static float rot = 0;
    rot+=0.01;
    if(rot>360){rot=0;}

    m_object->GetTransform().LoadIdentity();
    m_object->GetTransform().Translate(0.0f,0.0f,-4.5f);
    m_object->GetTransform().Rotate(rot,0.0f,0.2f,0.0f);
    m_object->GetTransform().Scale(2.0f,2.0f,2.0f);
    m_object->Update(m_screenWidth,m_screenHeight);
}

// Render
// The render function gets called once per loop
void SDLGraphicsProgram::Render(std::string s){
    // Initialize clear color
    // This is the background of the screen.
    glViewport(0, 0, m_screenWidth, m_screenHeight);
    glClearColor( 0.2f, 0.2f, 0.2f, 1.f );

	if(decimateby > lastDecimate) {
		m_object->decimatePercentage(decimateby);
		m_object->Update(m_screenWidth,m_screenHeight);
	}
	lastDecimate = decimateby;

    // TODO: Read this
    // Clear color buffer and Depth Buffer
    // Remember that the 'depth buffer' is our
    // z-buffer that figures out how far away items are every frame
    // and we have to do this every frame!
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	  //Number of indices to render
	int num_inds = m_object->getIndiceData().size();


    // TODO: Read this
    // Clear color buffer and Depth Buffer
    // Remember that the 'depth buffer' is our
    // z-buffer that figures out how far away items are every frame
    // and we have to do this every frame!
  	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	if(wireframeOn) {
		//Render Wireframe model
	num_inds = m_object->getIndiceData().size();
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	glLineWidth( 3.0f );
	}
	else {
		//Render Fill model
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		num_inds = m_object->getIndiceData().size();
	}

    // Render our object
    m_object->Render();
    // Note: This line is equivalent to:
    // (*m_object).Render(); // But the easier style
    // is to use the '->' which dereferences and then
    // selects the field/function with the '.' operator.
   
    SDL_Delay(50); 
}


//Loops forever!
void SDLGraphicsProgram::Loop(){
    // Main loop flag
    // If this is quit = 'true' then the program terminates.
    bool quit = false;
    // Event handler that handles various events in SDL
    // that are related to input and output
    SDL_Event e;
    // Enable text input
    SDL_StartTextInput();
	std::string s = "";
    // While application is running
    while(!quit){
     	 //Handle events on queue
		while(SDL_PollEvent( &e ) != 0){
        	// User posts an event to quit
	        // An example is hitting the "x" in the corner of the window.
    	    if(e.type == SDL_QUIT){
        		quit = true;
	        }
			if(e.key.type == SDL_KEYDOWN){
				switch(e.key.keysym.sym)
				{
					case(SDLK_w):
						s = "w"; //wireframe
						wireframeOn = ! wireframeOn;
						break;

					case(SDLK_q):
						s = "q"; //quit
						quit = true;
						break;

					case(SDLK_1):
						decimateby = 0.1;
						break;

					case(SDLK_2):
						decimateby = 0.2;
						break;

					case(SDLK_3):
						decimateby = 0.3;
						break;

					case(SDLK_4):
						decimateby = 0.4;
						break;

					case(SDLK_5):
						decimateby = 0.5;
						break;

					case(SDLK_6):
						decimateby = 0.6;
						break;
					
					case(SDLK_7):
						decimateby = 0.7;
						break;
					
					case(SDLK_8):
						decimateby = 0.8;
						break;

					case(SDLK_9):
						decimateby = 0.9;
						break;

					case(SDLK_0):
						decimateby = 0.99;
						break;

					case(SDLK_SPACE):
						if(currentObjectnum == 3) {
							currentObjectnum = 0;
						}
						else{
							currentObjectnum += 1;
						}
						switch(currentObjectnum){
							case(0):
							    m_object1->MakeGeometry("./../../common/objects/bunny_centered.obj");
								m_object = m_object1;
								lastDecimate = 0;
								decimateby = 0;
								break;
							case(1):
							    m_object2->MakeGeometry("./../../common/objects/monkey_centered.obj");
								m_object = m_object2;
								lastDecimate = 0;
								decimateby = 0;
								break;
							case(2):
							    m_object3->MakeGeometry("./../../common/objects/capsule/capsule.obj");
								m_object = m_object3;
								lastDecimate = 0;
								decimateby =0;
								break;
							case(3):
							    m_object4->MakeGeometry("./../../common/objects/3dScan2.obj");
								m_object = m_object4;
								lastDecimate = 0;
								decimateby = 0;
								break;
				      }
						break;
					default:
						s = ""; 
						break;
				}
        	}
      	} // End SDL_PollEvent loop.

		// Update our scene
		Update();
		// Render using OpenGL
	    Render(s); 	// TODO: potentially move this depending on your logic
					// for how you handle drawing a triangle or rectangle.
      	//Update screen of our specified window
      	SDL_GL_SwapWindow(GetSDLWindow());
    }

    //Disable text input
    SDL_StopTextInput();
}


// Get Pointer to Window
SDL_Window* SDLGraphicsProgram::GetSDLWindow(){
  return m_window;
}

// Helper Function to get OpenGL Version Information
void SDLGraphicsProgram::GetOpenGLVersionInfo(){
	SDL_Log("(Note: If you have two GPU's, make sure the correct one is selected)");
	SDL_Log("Vendor: %s",(const char*)glGetString(GL_VENDOR));
	SDL_Log("Renderer: %s",(const char*)glGetString(GL_RENDERER));
	SDL_Log("Version: %s",(const char*)glGetString(GL_VERSION));
	SDL_Log("Shading language: %s",(const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
}
