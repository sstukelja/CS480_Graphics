#include "room.h"
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

Room::Room( string fileName, btDiscreteDynamicsWorld* dynamicsWorld ) : Model( fileName )
   {
  
   }

void Room::Update( btDiscreteDynamicsWorld* dynamicsWorld, unsigned int dt )
   {
    // Initialize function/variables
    glm::mat4 rotate;
    glm::mat4 trans;
    
    trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0));
    
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(10.0f));

    // Rotate the model
    rotate = glm::rotate(glm::mat4(1.0f), 92.7f, glm::vec3(0.0, 1.0, 0.0));
    
    // Apply transformations to model matrix
    model = trans * scale;
   } 