#include "graphics.h"
#include <vector>
#include <fstream>
#include <time.h>
#include <string>

using namespace std;

Graphics::Graphics( btDiscreteDynamicsWorld* DynamicsWorld )
{
 dynamicsWorld = DynamicsWorld;
}

Graphics::~Graphics()
{

}

bool Graphics::Initialize(int width, int height, std::string fNames[] )
{
  // Initialize function/variables
  bool validObj;
  vector<Vertex> vertices;
  vector<unsigned int> indices;
  string gouraud[ 2 ];
  string phong[ 2 ];
  
  gouraud[ 0 ] = "v_pVertex.glsl";
  gouraud[ 1 ] = "f_pVertex.glsl";
  
  phong[ 0 ] = "v_pFrag.glsl";
  phong[ 1 ] = "f_pFrag.glsl";
  
  // Used for the linux OS
  #if !defined(__APPLE__) && !defined(MACOSX)
    // cout << glewGetString(GLEW_VERSION) << endl;
    glewExperimental = GL_TRUE;

    auto status = glewInit();

    // This is here to grab the error that comes from glew init.
    // This error is an GL_INVALID_ENUM that has no effects on the performance
    glGetError();

    //Check for error
    if (status != GLEW_OK)
    {
      std::cerr << "GLEW Error: " << glewGetErrorString(status) << "\n";
      return false;
    }
  #endif

  // For OpenGL 3
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // Init Camera
  m_camera = new Camera();
  if(!m_camera->Initialize(width, height))
  {
    printf("Camera Failed to Initialize\n");
    return false;
  }

  // Load the models
  string objFile = "PA10PinballMachineNoBumper.obj";
  m_ground = new Ground( objFile, dynamicsWorld );

  objFile = "PA10Ball.obj";
  m_sphere = new Sphere( objFile, dynamicsWorld );

  objFile = "spring.obj";
  m_cylinder = new Cylinder(objFile, dynamicsWorld);

  objFile = "PA10RightPaddle.obj";
  m_rpaddle = new RPaddle(objFile, dynamicsWorld);

  objFile = "PA10LeftPaddle2.obj";
  m_lpaddle = new LPaddle(objFile, dynamicsWorld);

  objFile = "backboard.obj";
  m_back = new Back(objFile, dynamicsWorld);

  objFile = "mashroom_new.obj";
  m_rbumper = new RBumper(objFile, dynamicsWorld);

  objFile = "mashroom_new.obj";
  m_lbumper = new LBumper(objFile, dynamicsWorld);

  objFile = "mashroom_new.obj";
  m_mbumper = new MBumper(objFile, dynamicsWorld);

  // Set up the shaders
  m_shaderGouraud = new Shader( gouraud );
  if(!m_shaderGouraud->Initialize( ))
  {
    printf("Gouraud Shader Failed to Initialize\n");
    return false;
  }

  // Set up the shaders
  m_shaderPhong = new Shader( phong );
  if(!m_shaderPhong->Initialize())
  {
    printf("Phong Shader Failed to Initialize\n");
    return false;
  }

  // Add the vertex shaders
  if(!m_shaderGouraud->AddShader(GL_VERTEX_SHADER))
  {
    printf("Gouraud Vertex Shader failed to Initialize\n");
    return false;
  }

  if(!m_shaderPhong->AddShader(GL_VERTEX_SHADER))
  {
    printf("Phong Vertex Shader failed to Initialize\n");
    return false;
  }

  // Add the fragment shaders
  if(!m_shaderGouraud->AddShader(GL_FRAGMENT_SHADER))
  {
    printf("Gouraud Fragment Shader failed to Initialize\n");
    return false;
  }

  if(!m_shaderPhong->AddShader(GL_FRAGMENT_SHADER))
  {
    printf("Phong Fragment Shader failed to Initialize\n");
    return false;
  }

  // Connect the programs
  if(!m_shaderGouraud->Finalize())
  {
    printf("Gouraud Program to Finalize\n");
    return false;
  }

  if(!m_shaderPhong->Finalize())
  {
    printf("Phong Program to Finalize\n");
    return false;
  }

  // Get matrices for active shader
  if( fNames[ 0 ] == "gouraud" || fNames[ 0 ] == "Gouraud" )
     {
      m_shader = m_shaderGouraud;
     }
  else
     {
      m_shader = m_shaderPhong;
      }

  // Locate the projection matrix in the shader
  m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
  if (m_projectionMatrix == INVALID_UNIFORM_LOCATION) 
  {
    printf("m_projectionMatrix not found\n");
    return false;
  }

  // Locate the view matrix in the shader
  m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
  if (m_viewMatrix == INVALID_UNIFORM_LOCATION) 
  {
    printf("m_viewMatrix not found\n");
    return false;
  }

  // Locate the model matrix in the shader
  m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");
  if (m_modelMatrix == INVALID_UNIFORM_LOCATION) 
  {
    printf("m_modelMatrix not found\n");
    return false;
  }

  // Start the correct program
  m_shader->Enable();
      
  //enable depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
    
  // Initialize lights
  Light light;
  light.LightPosition = glm::vec4( 0.1,10.0,-0.1,0.0);
  light.AmbientProduct = glm::vec4(0.2,0.2,0.2,1.0); 
  light.DiffuseProduct = glm::vec4(0.1,0.1,0.9,1.0); 
  light.SpecularProduct = glm::vec4(0.9,0.1,0.1,1.0); 
  light.coneAngle = 180.0;
  light.coneDirection = glm::vec3( 0.0, -1.0, 0.0 );
  light.brightness = 1.0;

  Light light2;
  light2.LightPosition = glm::vec4( 0.5,10.0,-1.5,1.0);
  light2.AmbientProduct = glm::vec4(0.1,0.1,0.1,1.0); 
  light2.DiffuseProduct = glm::vec4(0.4,0.4,0.4,1.0); 
  light2.SpecularProduct = glm::vec4(0.6,0.6,0.6,1.0); 
  light2.coneAngle = 10.0;
  light2.coneDirection = glm::vec3( 0.0, -1.0, -1.5 );
  light2.brightness = 1.0;
  
  
  lights.push_back(light2);
  lights.push_back(light);
 
  return true;
}

void Graphics::Update(unsigned int dt, string motion[])
{
  // Check for shader swap
  if( motion[ 1 ] == "GOURAUD" || motion[ 1 ] == "PHONG" )
     {
      swapShaders( motion[ 1 ] );
     }
  else if( motion[ 1 ] != "NONE" )
     {
      adjustLighting( motion[ 1 ] );
     }
  if( motion[0] == "UP")
     {
        m_sphere->Start();
     }
  if( motion[0] == "RIGHT")
     {
   //     m_rpaddle->Swing();
        m_sphere->Right();
     }
  if( motion[0] == "LEFT")
     {
   //     m_lpaddle->Swing();
        m_sphere->Left();
     }
     
  double dTime = (double) dt / 500;
  
  // Check if ball needs to be reset
  if(gutter.distance(getSphereCOM()) < 0.5)
  {
        delete m_sphere;
        string objFile;
        objFile = "PA10Ball.obj";
        m_sphere = new Sphere( objFile, dynamicsWorld );
        BallNum++;
        if(BallNum != 3)
        {
        cout << "Ball: " << BallNum+1 << endl;
        }
  }

  // Check if Score changed
  if(lbumper.distance(getSphereCOM()) < 4.6)
  {
        Score += 25;
        cout << "Score: " << Score << endl;
  }

  if(rbumper.distance(getSphereCOM()) < 4.6)
  {
        Score += 25;
        cout << "Score: " << Score << endl;
  }

  // Update the dynamics world
  dynamicsWorld->stepSimulation( dTime, 1 );
  
  // Update the objects
  m_ground->Update( dynamicsWorld, dt );
  m_sphere->Update( dynamicsWorld, dt );
  m_cylinder->Update(dynamicsWorld, dt);
  m_rpaddle->Update(dynamicsWorld, dt);
  m_lpaddle->Update(dynamicsWorld, dt);
  m_back->Update(dynamicsWorld, dt);
  m_rbumper->Update(dynamicsWorld, dt);
  m_lbumper->Update(dynamicsWorld, dt);
  m_mbumper->Update(dynamicsWorld, dt);
}

void Graphics::swapShaders( string shader )
   {
    if( shader == "PHONG" )
       {
        m_shader = m_shaderPhong;
       }
    else if( shader == "GOURAUD" )
       {
        m_shader = m_shaderGouraud;
       }   

    m_projectionMatrix = m_shader->GetUniformLocation("projectionMatrix");
    m_viewMatrix = m_shader->GetUniformLocation("viewMatrix");
    m_modelMatrix = m_shader->GetUniformLocation("modelMatrix");

    // Start the correct program
    m_shader->Enable();
   }
   

void Graphics::Render()
{
  //clear the screen
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


  // Send in the projection and view to the shader
  glUniformMatrix4fv(m_projectionMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetProjection())); 
  glUniformMatrix4fv(m_viewMatrix, 1, GL_FALSE, glm::value_ptr(m_camera->GetView())); 

  // Render the objects
  glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_ground->getModel()));
  setLightingUniforms( m_ground );
  m_ground->Draw();

  glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_sphere->getModel()));
  setLightingUniforms( m_sphere );
  m_sphere->Draw();

  glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_cylinder->getModel()));
  setLightingUniforms( m_cylinder );
  m_cylinder->Draw();

  glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_rpaddle->getModel()));
  setLightingUniforms( m_rpaddle );
  m_rpaddle->Draw();

  glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_lpaddle->getModel()));
  setLightingUniforms( m_lpaddle );
  m_lpaddle->Draw();

  glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_back->getModel()));
  setLightingUniforms( m_back );
  m_back->Draw();

  glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_rbumper->getModel()));
  setLightingUniforms( m_rbumper );
  m_rbumper->Draw();

  glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_lbumper->getModel()));
  setLightingUniforms( m_lbumper );
  m_lbumper->Draw();

  glUniformMatrix4fv(m_modelMatrix, 1, GL_FALSE, glm::value_ptr(m_mbumper->getModel()));
  setLightingUniforms( m_mbumper );
  m_mbumper->Draw();

  // Get any errors from OpenGL
  auto error = glGetError();
  if ( error != GL_NO_ERROR )
  {
    string val = ErrorString( error );
    std::cout<< "Error initializing OpenGL! " << error << ", " << val << std::endl;
  }
}

void Graphics::setLightingUniforms( Model* object )
   {
    // Set number of lights
    GLuint loc = m_shader->GetUniformLocation( "numLights" );
    int numLights = lights.size();
    glUniform1i( loc, numLights );
    string locName;

    loc = m_shader->GetUniformLocation( "Shininess" );
    glUniform1f( loc, object->getShininess() );

    for( int index = 0; index < lights.size(); index++ )
       {
        locName = "lights[" + to_string(index) + "].LightPosition";
        loc = m_shader->GetUniformLocation( locName.c_str() );
        glUniform4fv( loc, 1, glm::value_ptr( lights[ index ].LightPosition ) );

        locName = "lights[" + to_string(index) + "].AmbientProduct";
        loc = m_shader->GetUniformLocation( locName.c_str() );
        glUniform4fv( loc, 1, glm::value_ptr( lights[ index ].AmbientProduct ) );

        locName = "lights[" + to_string(index) + "].DiffuseProduct";
        loc = m_shader->GetUniformLocation( locName.c_str() );
        glUniform4fv( loc, 1, glm::value_ptr( lights[ index ].DiffuseProduct ) );

        locName = "lights[" + to_string(index) + "].SpecularProduct";
        loc = m_shader->GetUniformLocation( locName.c_str() );
        glUniform4fv( loc, 1, glm::value_ptr( lights[ index ].SpecularProduct ) );

        locName = "lights[" + to_string(index) + "].coneAngle";
        loc = m_shader->GetUniformLocation( locName.c_str() );
        glUniform1f( loc, lights[ index ].coneAngle );
        
        locName = "lights[" + to_string(index) + "].coneDirection";
        loc = m_shader->GetUniformLocation( locName.c_str() );
        glUniform3fv( loc, 1, glm::value_ptr( lights[ index ].coneDirection ) );

        locName = "lights[" + to_string(index) + "].brightness";
        loc = m_shader->GetUniformLocation( locName.c_str() );
        glUniform1f( loc, lights[ index ].brightness );
       }
   }

void Graphics::adjustLighting( string control )
   {
    int index;
    
    if( control == "I AMBIENT BRIGHT" )
       {
        lights[ 1 ].AmbientProduct += glm::vec4( -0.1, -0.1, -0.1, 0.0 );       
       }
    else if( control == "D AMBIENT BRIGHT" )
       {
        lights[ 1 ].AmbientProduct += glm::vec4( 0.1, 0.1, 0.1, 0.0 ); 
       }
    else if( control == "I SPEC" )
       {
        m_ground->adjustShininess( "UP" );
        m_sphere->adjustShininess( "UP" );
        m_cylinder->adjustShininess( "UP" );
        m_rpaddle->adjustShininess( "UP" );
        m_lpaddle->adjustShininess( "UP" );
        m_back->adjustShininess( "UP" );
        m_rbumper->adjustShininess( "UP" );
        m_lbumper->adjustShininess( "UP" );
        m_mbumper->adjustShininess( "UP" );
       }
    else if( control == "D SPEC" )
       {
        m_ground->adjustShininess( "DOWN" );
        m_sphere->adjustShininess( "DOWN" );
        m_cylinder->adjustShininess( "DOWN" );
        m_rpaddle->adjustShininess( "DOWN" );
        m_lpaddle->adjustShininess( "DOWN" );
        m_back->adjustShininess( "DOWN" );
        m_rbumper->adjustShininess( "DOWN" );
        m_lbumper->adjustShininess( "UP" );
        m_mbumper->adjustShininess( "UP" );
       }
    else if( control == "I SPOT SIZE" )
       {
        lights[ 0 ].coneAngle += 1;
       }
    else if( control == "D SPOT SIZE" )
       {
        lights[ 0 ].coneAngle -= 1;
       }
   }
    
btVector3 Graphics::getSphereCOM()
{
    return m_sphere->getCOM();
}

int Graphics::getBallNum()
{
    return BallNum;
}

int Graphics::getScore()
{
    return Score;
}
   
std::string Graphics::ErrorString(GLenum error)
{
  if(error == GL_INVALID_ENUM)
  {
    return "GL_INVALID_ENUM: An unacceptable value is specified for an enumerated argument.";
  }

  else if(error == GL_INVALID_VALUE)
  {
    return "GL_INVALID_VALUE: A numeric argument is out of range.";
  }

  else if(error == GL_INVALID_OPERATION)
  {
    return "GL_INVALID_OPERATION: The specified operation is not allowed in the current state.";
  }

  else if(error == GL_INVALID_FRAMEBUFFER_OPERATION)
  {
    return "GL_INVALID_FRAMEBUFFER_OPERATION: The framebuffer object is not complete.";
  }

  else if(error == GL_OUT_OF_MEMORY)
  {
    return "GL_OUT_OF_MEMORY: There is not enough memory left to execute the command.";
  }
  else
  {
    return "None";
  }
}

Camera* Graphics::getCamera()
   {
    return m_camera;
   }

