#include "rpaddle.h"
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

RPaddle::RPaddle( string fileName, btDiscreteDynamicsWorld* dynamicsWorld ) : Model( fileName )
   {
    // Create paddle collision shape
    btCollisionShape* paddleShape = new btBoxShape( btVector3( 1, 10, 4 ) );
        
    // Create paddle motion state, place in socket
    btDefaultMotionState* paddleMotionState = new btDefaultMotionState( 
    btTransform( btQuaternion( 0, 0, 0, 1 ), btVector3( 0, 1, 0 ) ) );            
            
    // Create Cylinder rigid body
    btScalar mass = 1;
    btVector3 paddleInertia = btVector3( 0, 0, 0 );
    paddleShape->calculateLocalInertia( mass, paddleInertia );
    btRigidBody::btRigidBodyConstructionInfo paddleRigidBodyCI( 
                                mass, paddleMotionState, paddleShape, paddleInertia );
    rigidBody = new btRigidBody( paddleRigidBodyCI );

    rigidBody->setLinearFactor(btVector3( 0, 0, 0 ) );
    rigidBody->setAngularFactor(btVector3( 0, 1, 0 ) );
    
    rigidBody->setRestitution( .001 );
                        
    // Add paddle to world
    dynamicsWorld->addRigidBody( rigidBody );    
   }

void RPaddle::Update( btDiscreteDynamicsWorld* dynamicsWorld, unsigned int dt )
   {
    btTransform trans;
    btScalar m[ 16 ];
    
    rigidBody->getMotionState()->getWorldTransform( trans );
    
    trans.getOpenGLMatrix( m );
    
    model = glm::make_mat4( m );
   } 