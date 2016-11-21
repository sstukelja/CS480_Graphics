#include "rbumper.h"
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

RBumper::RBumper( string fileName, btDiscreteDynamicsWorld* dynamicsWorld ) : Model( fileName, m_objTriMesh )
   {
    // Create bumper collision shape
    btCollisionShape* bumperShape = new btBvhTriangleMeshShape( m_objTriMesh, true);
        
    // Create bumper motion state
    btDefaultMotionState* bumperMotionState = new btDefaultMotionState( 
    btTransform( btQuaternion( 0, 0, 0, 1), btVector3( -10, 1, 20 ) ) );            
            
    // Create bumper rigid body
    btScalar mass = 0;
    btVector3 bumperInertia = btVector3( 0, 0, 0 );
    bumperShape->calculateLocalInertia( mass, bumperInertia );
    btRigidBody::btRigidBodyConstructionInfo bumperRigidBodyCI( 
                                mass, bumperMotionState, bumperShape, bumperInertia );
    rigidBody = new btRigidBody( bumperRigidBodyCI );

    rigidBody->setLinearFactor(btVector3( 0, 0, 0 ) );
    rigidBody->setAngularFactor(btVector3( 0, 0, 0 ) );
    
                        
    // Add bumper to world
    dynamicsWorld->addRigidBody( rigidBody, COL_BUMPER, bumperCollidesWith );    
   }

void RBumper::Update( btDiscreteDynamicsWorld* dynamicsWorld, unsigned int dt )
   {
    btTransform trans;
    btScalar m[ 16 ];

    rigidBody->forceActivationState(ACTIVE_TAG);
    
    rigidBody->getMotionState()->getWorldTransform( trans );
    
    trans.getOpenGLMatrix( m );
    
    model = glm::make_mat4( m );
   } 
