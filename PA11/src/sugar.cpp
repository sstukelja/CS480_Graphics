#include "Sugar.h"
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

Sugar::Sugar( string fileName, btDiscreteDynamicsWorld* dynamicsWorld ) : Model( fileName )
   {
    // Create Sugar collision shape
    btCollisionShape* fallShape = new btBoxShape( 2 );
        
    // Create Sugar motion state, place 50 meters above ground
    btDefaultMotionState* fallMotionState = new btDefaultMotionState( 
    btTransform( btQuaternion( 0, 0, 0, 1 ), btVector3( 81.2, 80, 92 ) ) );            
            
    // Create Sugar rigid body
    btScalar mass = 10;
    btVector3 fallInertia = btVector3( 0, 0, 0 );
    fallShape->calculateLocalInertia( mass, fallInertia );
    btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI( 
                                mass, fallMotionState, fallShape, fallInertia );
    rigidBody = new btRigidBody( fallRigidBodyCI );

    rigidBody->setLinearFactor(btVector3( 1, 1, 1 ) );
    rigidBody->setAngularFactor(btVector3( 0, 1, 0 ) );

    rigidBody->setRestitution(0.9);
                        
    // Add Sugar to world
    dynamicsWorld->addRigidBody( rigidBody, COL_BALL, ballCollidesWith );    
   }

Sugar::~Sugar()
{
}

void Sugar::Update( btDiscreteDynamicsWorld* dynamicsWorld, unsigned int dt )
   {

    btTransform trans;
    btScalar m[ 16 ];

    rigidBody->forceActivationState(ACTIVE_TAG);
    
    rigidBody->getMotionState()->getWorldTransform( trans );
    
    trans.getOpenGLMatrix( m );
    
    model = glm::make_mat4( m );
   } 

void Sugar::Start()
{
    if(start.distance(rigidBody->getCenterOfMassPosition()) < 1)
    {
        rigidBody->applyCentralImpulse( btVector3(0,0,1000));
    }
}

btVector3 Sugar::getCOM()
{
    return rigidBody->getCenterOfMassPosition();
}
