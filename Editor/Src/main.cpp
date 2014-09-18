#include <GLUT/glut.h>

#include "Render/Mesh.h"

#include "System.h"

#include "Render/MeshHelper.h"

#include "FileSystem.h"
#include "DataStream.h"
#include "Render/MeshSerializer.h"
#include "ResourceManager.h"
#include "Render/TextureSerializer.h"


#include "Render/Picture.h"

#include "Render/Renderer.h"

#include "Render/MaterialHelper.h"

#include "Render/MaterialSerializer.h"
#include <stdio.h>

#include "GlobalVariables.h"

#include "AssetManager.h"


#include "Skeleton.h"

using namespace WhiteBox;

GLuint textureID, textureID2;

WhiteBox::CResourcePointer< WhiteBox::CTexture > pTex;
WhiteBox::CMesh* pMesh1 = NULL, *pMesh2 = NULL;
 	WhiteBox::CVertexFormat decl;

	
	WhiteBox::CResourcePointer< WhiteBox::CMesh > pMesh;
	
	
	float rot = 0.0f;
	
	CSkeleton skel;
	
	
	float phase = 0.0f;


static const size_t vert = 20;
static const size_t hori = 20;

float radius = 0.7f;
float height = 3.0f;

Vec3 points[ hori * vert ];
Vec3 points2[ hori * vert ];
float weights[ hori * vert ];



void display(void) {

    //clear white, draw with black
    glClearColor(0, 0, 0, 0);
//    glColor3f(1.0f, 1.0f, 1.0f);
	
                        // Enable Texture Mapping ( NEW )
    glShadeModel(GL_SMOOTH);                        // Enable Smooth Shading
    glClearDepth(1.0f);                         // Depth Buffer Setup
    glEnable(GL_DEPTH_TEST);                        // Enables Depth Testing
    glDepthFunc(GL_LEQUAL);                         // The Type Of Depth Testing To Do
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);          // Really Nice Perspectiv	

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glEnable(GL_TEXTURE_2D);
		
	/*
	GLfloat mat[] = { 1.0, 1.0, 1.0, 1.0 };
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	GLfloat zero[] = { 0,0,0,1.f};
   
   GLfloat ambient[] = {1.0f, 1.0f, 1.0f, 1.0f};
   glLightfv( GL_LIGHT0, GL_AMBIENT, zero);
   glLightfv( GL_LIGHT0, GL_DIFFUSE, ambient);
   glLightfv( GL_LIGHT0, GL_SPECULAR, zero);*/	
	
	glLoadIdentity();

	//rot += 0.1f;
	//glBindTexture(GL_TEXTURE_2D, textureID);
	//glScalef(4,4,4); 
	glTranslatef(0.0f,-1.0f, -9.6f);  //
	//glTranslatef(0.0f,-10.0f, -40.6f); 

		//	glRotatef( rot, 0, 1,0  );
	//if(pMesh1);
//pMesh1->Render();

if (pMesh )
{
//	WhiteBox::gSystem->pRenderer->BindTexture( pTex->GetTextureId(), 0 );

	//pMesh->Render();
}

	//glRotatef( -rot, 0, 1, 0 );
//	glTranslatef( 1.5f, 0.0f, 6.0f);

//glTranslatef(1.5f,0.0f,-6.0f);
//	glRotatef( rot, 0, 1,0  );
	//if(pMesh2)
//pMesh2->Render();
//	glRotatef( -rot, 0, 1, 0 );


/*
glColorPointer(3, GL_FLOAT, decl.GetSize(), (void*)decl.GetSingleElementOffset( WhiteBox::CVertexDeclaration::eSE_Color ) );	
//glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(SVertex), BUFFER_OFFSET(ColorOffset));
glVertexPointer(3, GL_FLOAT, decl.GetSize(), (void*)decl.GetSingleElementOffset( WhiteBox::CVertexDeclaration::eSE_Position ) );

glEnableClientState(GL_VERTEX_ARRAY);
glEnableClientState(GL_COLOR_ARRAY);

glDrawArrays(GL_TRIANGLES, 0, 3);

glDisableClientState(GL_COLOR_ARRAY);
glDisableClientState(GL_VERTEX_ARRAY);*/
	//pMesh2->Render();
	
	/*
	glBegin(GL_TRIANGLES);                      // Drawing Using Triangles
		glVertex3f( 0.0f, 1.0f, 0.0f);              // Top
		glVertex3f(-1.0f,-1.0f, 0.0f);              // Bottom Left
		glVertex3f( 1.0f,-1.0f, 0.0f);              // Bottom Right
	glEnd();  */
	
	//WhiteBox::gSystem->pRenderer->BindTexture( pTex->GetTextureId(), 0 );
	
	//WhiteBox::gSystem->pRenderer->BindTexture( new int(12), 0 );
	//WhiteBox::gSystem->pRenderer->BindTexture( new int(1), 0 );
	
	//glTranslatef(3.0f,0.0f, -1.0f) ;//0.0f); 		
		/*
	glBegin(GL_QUADS);                      // Draw A Quad
        glVertex3f(-1.0f, 1.0f, 0.0f);              // Top Left
        glVertex3f( 1.0f, 1.0f, 0.0f);              // Top Right
        glVertex3f( 1.0f,-1.0f, 0.0f);              // Bottom Right
        glVertex3f(-1.0f,-1.0f, 0.0f);              // Bottom Left
    glEnd(); */
	
	
//	glColor3f( 0.0f, 0.0f, 1.0f );
	
//glTranslatef(-8,0.0f,0.0f); 
//  glMaterialfv(GL_FRONT, GL_AMBIENT, mat);	

	phase += 0.5f;
	
	float angle = Cos( Degree(phase) ) * 40.0f;
	float angle3 = Cos( Degree(phase * 0.2f) ) * 90.0f;


if (true)
{
	size_t count = skel.GetBones().size();
	
	skel.ComputeInvertedGlobalBindPose();
	
	CPose pose;
	pose.m_boneTransforms.resize( count );
	pose.m_boneTransforms[ 1 ].rotation = Quat::CreateRotX( Degree(angle3 ) ) * Quat::CreateRotZ( Degree(angle) );
	//pose.m_boneTransforms[ 1 ].position.x = 0.5f;
	skel.ConvertFromBindToLocalSpace( pose );
	skel.ComputeGlobalPose( pose, pose );
	
	printf( "Skel\n" );
	for( size_t i=0 ; i < count ; ++i )
	{
		CBone& bone = skel.GetBones()[ i ];
		
		if ( bone.GetParentIndex() >= 0 )
		{
			Vec3 parentPos = pose.m_boneTransforms[ bone.GetParentIndex() ].position;
			gSystem->pRenderer->DrawLine( parentPos, pose.m_boneTransforms[ i ].position, Color::White );
			
		}
		
		printf( "Bone %s, father:%d, Pos(%.2f,%.2f,%.2f)\n", bone.GetName().c_str(), bone.GetParentIndex(), pose.m_boneTransforms[ i ].position.x, pose.m_boneTransforms[ i ].position.y, pose.m_boneTransforms[ i ].position.z );
	}
	
	CPose skinPose;
	skel.ComputeSkinningPose( pose, skinPose );
	
	for(int i=0 ; i<vert*hori ; ++i)
	{
		float w = points[ i ].y / 3.0f;
	
		points2[ i ] = (1.0f - w)*(skinPose.m_boneTransforms[ 0] * points[ i ]) + w*(skinPose.m_boneTransforms[ 2] * points[ i ]);
		
		
		
	}
	
	for( size_t i=0 ; i < vert ; ++i )
	for( size_t j=0 ; j < hori-1 ; ++j )
	{
		Vec3& pt1 = points2[ hori * i + j ];
		Vec3& pt2 = points2[ hori * i + j + 1 ];
		
		gSystem->pRenderer->DrawLine( pt1, pt2, Color::Blue );

		
		
	}
	
	printf( "   %.2f\n", points[ 0].y );
	printf( "   %.2f\n", points2[ 0].x ); 
   
}

	glTranslatef(0.55f,0.0f,0.0f); 		
    //this draws a square using vertices
  /*  glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(0, 128);
    glVertex2i(128, 128);
    glVertex2i(128, 0);
    glEnd();*/

    //a more useful helper
//    glRecti(200, 200, 250, 250);

 
    glutSwapBuffers();
 

}

void reshape(int width, int height) {

    glViewport(0, 0, width, height);
	
	float w = 2.41421f; //1.0f / 0.4f;
	
	
	Degree fov( 45.0f );
	w = 1.0f / Tan( fov * 0.5f );
	float h = (w * float(height)) / float(width);
	
	WhiteBox::Matrix44 projMatrix;
	WhiteBox::gSystem->pRenderer->ComputeProjectionMatrix( 0.1f, 100.0f, h, w, projMatrix );
	WhiteBox::gSystem->pRenderer->SetProjectionMatrix( projMatrix );
	
 /*   glMatrixMode(GL_PROJECTION);
    glLoadIdentity();


    //set the coordinate system, with the origin in the top left
    gluOrtho2D(0, width, height, 0);
    glMatrixMode(GL_MODELVIEW);
	*/
//	glMatrixMode(GL_PROJECTION);                        // Select The Projection Matrix
  //  glLoadIdentity();                           // Reset The Projection Matrix
 
    // Calculate The Aspect Ratio Of The Window
//    gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);
 
    glMatrixMode(GL_MODELVIEW);                     // Select The Modelview Matrix
    glLoadIdentity();  	

}

void idle(void) {

    glutPostRedisplay();
}



int main(int argc, char *argv) {

WhiteBox::SSystem::Init();
WhiteBox::SGlobalVariables::Init();


//	WhiteBox::CAssetManager assMan;
	//assMan.Export( "Assets", "Resources" );

//	WhiteBox::gVars->pResourceManager->ParseResources( "Resources" );
	

	skel.AddBone( "root", "", Transform() );
	skel.AddBone( "child1", "root", Transform( Vec3(0.0f,1.5f,0.0f), Quat() ) );
	skel.AddBone( "child2", "child1", Transform( Vec3(0.0f,1.5f,0.0f), Quat() ) );
	/*
	WhiteBox::gVars->pResourceManager->AddResource( "4bc51fe0_dds.mat" );
	WhiteBox::gVars->pResourceManager->AddResource( "4bc51fe0.jpg" );
	
	WhiteBox::gVars->pResourceManager->AddResource( "6fcfa7ef_dds.mat" );
	WhiteBox::gVars->pResourceManager->AddResource( "6fcfa7ef.jpg" );
	
	WhiteBox::gVars->pResourceManager->AddResource( "18d679fb_dds.mat" );
	WhiteBox::gVars->pResourceManager->AddResource( "18d679fb.jpg" );
	
	WhiteBox::gVars->pResourceManager->AddResource( "30ccb2c6_dds.mat" );
	WhiteBox::gVars->pResourceManager->AddResource( "30ccb2c6.jpg" );
	
	WhiteBox::gVars->pResourceManager->AddResource( "772d019a_dds.mat" );
	WhiteBox::gVars->pResourceManager->AddResource( "772d019a.jpg" );
	
	WhiteBox::gVars->pResourceManager->AddResource( "774b1982_dds.mat" );
	WhiteBox::gVars->pResourceManager->AddResource( "774b1982.jpg" );
	
	WhiteBox::gVars->pResourceManager->AddResource( "946ea76f_dds.mat" );
	WhiteBox::gVars->pResourceManager->AddResource( "946ea76f.jpg" );
	
	WhiteBox::gVars->pResourceManager->AddResource( "3378c10d_dds.mat" );
	WhiteBox::gVars->pResourceManager->AddResource( "3378c10d.jpg" );
	
	WhiteBox::gVars->pResourceManager->AddResource( "691524bf_dds.mat" );
	WhiteBox::gVars->pResourceManager->AddResource( "691524bf.jpg" );
	
	WhiteBox::gVars->pResourceManager->AddResource( "ae6dcce1_dds.mat" );
	WhiteBox::gVars->pResourceManager->AddResource( "ae6dcce1.jpg" );
	
	WhiteBox::gVars->pResourceManager->AddResource( "c071add4_dds.mat" );
	WhiteBox::gVars->pResourceManager->AddResource( "c071add4.jpg" );
	
	WhiteBox::gVars->pResourceManager->AddResource( "f664140d_dds.mat" );
	WhiteBox::gVars->pResourceManager->AddResource( "f664140d.jpg" );*/
	
//	WhiteBox::CMeshHelper * msh = new WhiteBox::CMeshHelper();
	/*msh->Load( "Ezio.obj" );
	msh->SaveToFile( "Ezio.msh" );
	
	msh = new WhiteBox::CMeshHelper();
	msh->Load( "Goku.obj" );
	msh->SaveToFile( "Goku.msh" );

	*/

//	decl.AddSingleElement( WhiteBox::CVertexFormat::eSE_Position );
//	decl.AddSingleElement( WhiteBox::CVertexFormat::eSE_Color );
//	decl.Build();

    //a basic set up...
    glutInit(&argc, &argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(640, 480);

    //create the window, the argument is the title
    glutCreateWindow("GLUT program");

    //pass the callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(idle);
	
for( size_t i=0 ; i < vert ; ++i )
{
	for( size_t j=0 ; j < hori ; ++j )
	{
		Vec3& pt =points[ hori * i + j ];
		pt.x = Cos( Degree( j * (360.0f/hori) ) ) * radius;
		pt.z = Sin( Degree( j * (360.0f/hori) ) ) * radius;
		pt.y = ( float(i) / float(vert-1) ) * height;
	}
}	

	
	//pMesh = WhiteBox::gVars->pResourceManager->GetResource< WhiteBox::CMesh >( "Resources/Borderlands_Zero/zero.msh" ); //"Resources/Ezio/Ezio.msh" ); //"Resources/Vanquish/vanquish.msh" ); //"Resources/Goku/Goku SS2/Goku SS2.msh" );
	//pMesh = WhiteBox::gVars->pResourceManager->GetResource< WhiteBox::CMesh >( "Resources/Goku/Goku SS2/Goku SS2.msh" );

	
	//pTex = WhiteBox::gVars->pResourceManager->GetResource< WhiteBox::CTexture >( "tex" );
		
	//pTex = WhiteBox::gVars->pResourceManager->GetResource< WhiteBox::CTexture >( "dirtyinnersuit_d" );
		
	//	static WhiteBox::CMaterialPtr pMat = WhiteBox::gVars->pResourceManager->GetResource< WhiteBox::CMaterial >( "material" );
		
//	WhiteBox::gVars->pResourceManager->LoadQueriedResources();
	
	//printf( "Material shininess %.5f, emissive (%.5f,%.5f,%.5f,%.5f)\n", pMat->m_shininess, pMat->m_emissiveColor.x, pMat->m_emissiveColor.y, pMat->m_emissiveColor.z, pMat->m_emissiveColor.w );
	

//glGenTextures(1, &textureID);

//glBindTexture(GL_TEXTURE_2D, textureID);



	
//	glTexImage2D (GL_TEXTURE_2D, 0, GL_RGB,
//		    nWidth, nHeight, 0, GL_RGB,
//		    GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(pImage));

// Create one OpenGL texture


 
// "Bind" the newly created texture : all future texture functions will modify this texture



 
// Give the image to OpenGL

//	WhiteBox::CMaterialHelper matHelper;
//	matHelper.SaveToFile( "material.mat" );
 
	
//	pSubMesh->SetVertices( decl, 3, vertices );
//	pSubMesh->SetIndices( 3, indices );
	
	//pMesh = new WhiteBox::CMesh();
		//pMesh2 = msh->ConvertToMesh();
		/*
	WhiteBox::TFileHandle file = WhiteBox::gSystem->pFileSystem->OpenFile( "Ezio.msh", true, false );
	
	size_t fileSize = WhiteBox::gSystem->pFileSystem->GetFileSize( file );
	
	char* fileData = new char[ fileSize ];
	WhiteBox::gSystem->pFileSystem->Read( file, 1, fileSize, fileData );
	
	{
		WhiteBox::CDataStream dataStream( fileData, fileSize );
		WhiteBox::CMeshSerializer meshSerializer;
		
		pMesh1 = meshSerializer.Load( dataStream );
	}
	
	delete[] fileData;
	
	WhiteBox::gSystem->pFileSystem->CloseFile( file );
	
	file = WhiteBox::gSystem->pFileSystem->OpenFile( "vanquish.msh", true, false );
	
	fileSize = WhiteBox::gSystem->pFileSystem->GetFileSize( file );
	
	fileData = new char[ fileSize ];
	WhiteBox::gSystem->pFileSystem->Read( file, 1, fileSize, fileData );
	
	{
		WhiteBox::CDataStream dataStream( fileData, fileSize );
		WhiteBox::CMeshSerializer meshSerializer;
		
		pMesh2 = meshSerializer.Load( dataStream );
	}
	
	delete[] fileData;
	
	WhiteBox::gSystem->pFileSystem->CloseFile( file );	*/
		
		
	//	delete msh;
	//pMesh->AddSubMesh( pSubMesh );
	
	//pVB = new WhiteBox::CVertexBuffer( decl, 3, vertices );
	//pIB = new WhiteBox::CIndexBuffer( 3, indices );
	/*
	glGenBuffers( 1, &vbo );
	
	// if pData is NULL, only allocate memory (for future write)
	glBindBuffer( GL_ARRAY_BUFFER, vbo );
	glBufferData( GL_ARRAY_BUFFER, sizeof(float)*9, vertices, GL_STREAM_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
	glGenBuffers( 1, &ibo );
	
	// if pData is NULL, only allocate memory (for future write)
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*3, indices, GL_STREAM_DRAW );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );*/
		
	
	
/*	GLuint id = 0;	
	glGenBuffers( 1, &id );
	
	// if pData is NULL, only allocate memory (for future write)
	glBindBuffer( GL_ARRAY_BUFFER, id );
	glBufferData( GL_ARRAY_BUFFER, 9 * sizeof(GLfloat), vertices, GL_STREAM_DRAW );*/	
	
//	pVB = new WhiteBox::CVertexBuffer( decl, 3, vertices );
	

    glutMainLoop();
	
	//delete pMesh1;
	//delete pMesh2;

    //we never get here because glutMainLoop() is an infinite loop
    return 0;

}
