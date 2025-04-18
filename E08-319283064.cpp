/*
Pr�ctica 7: Iluminaci�n 1
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <math.h>

#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
#include "Sphere.h"
#include"Model.h"
#include "Skybox.h"

//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;

Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture pisoTexture;
Texture AgaveTexture;
Texture octaedro;

Model Kitt_M;
Model Llanta_M;
Model Blackhawk_M;

Model Carro;
Model Cofre;
Model Llanta;

Model Luminaria;

Skybox skybox;

//materiales
Material Material_brillante;
Material Material_opaco;


//Sphere cabeza = Sphere(0.5, 20, 20);
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;
static double limitFPS = 1.0 / 60.0;

// luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";


//funci�n de calculo de normales por promedio de v�rtices 
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}


void CreateObjects()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
		//	x      y      z			u	  v			nx	  ny    nz
			-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
			1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
	   0, 1, 2,
	   0, 2, 3,
	   4,5,6,
	   4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};

	Mesh* obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh* obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh* obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	Mesh* obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

}

void CrearDado()
{
	unsigned int octaedro_indices[] = {
		// 1
		0, 1, 2,
		// 2
		3, 4, 5,
		// 3
		6, 7, 8,
		// 4
		9, 10, 11,
		// 5
		12, 13, 14,
		// 6
		15, 16, 17,
		// 7
		18, 19, 20,
		// 8
		21, 22, 23,
	};

	GLfloat octaedro_vertices[] = {
		// 1 
		//x		y		z		S		T
		-0.5f,	0.0f,  0.5f,	0.02f,	0.35f,	0.0f,	-1.0f,	-1.0f,
		0.0f,	0.75f,  0.0f,	0.14f,	0.63f,	0.0f,	-1.0f,	-1.0f,
		0.5f,	0.0f,  0.5f,	0.27f,	0.35f,	0.0f,	-1.0f,	-1.0f,

		// 2
		//x		y		z		S		T
		0.5f,	0.0f,  0.5f,	0.412f,	0.64f,	-1.0f,	-1.0f,	0.0f,
		0.0f,	0.75f,  0.0f,	0.29f,	0.36f,	-1.0f,	-1.0f,	0.0f,
		0.5f,	0.0f,  -0.5f,	0.17f,	0.64f,	-1.0f,	-1.0f,	0.0f,

		// 3
		//x		y		z		S		T
		0.5f,	0.0f,  -0.5f,	0.30f,	0.34f,	0.0f,	-1.0f,	1.0f,
		0.0f,	0.75f,  0.0f,	0.43f,	0.65f,	0.0f,	-1.0f,	1.0f,
		-0.5f,	0.0f,  -0.5f,	0.55f,	0.34f,	0.0f,	-1.0f,	1.0f,

		// 4
		//x		y		z		S		T
		-0.5f,	0.0f,  -0.5f,	0.68f,	0.64f,	1.0f,	-1.0f,	0.0f,
		0.0f,	0.75f,  0.0f,	0.57f,	0.35f,	1.0f,	-1.0f,	0.0f,
		-0.5f,	0.0f,  0.5f,	0.46f,	0.64f,	1.0f,	-1.0f,	0.0f,

		// 5
		//x		y		z		S		T
		-0.5f,	0.0f,  0.5f,	0.84f,	0.35f,	0.0f,	1.0f,	-1.0f,
		0.0f,	-0.75f,  0.0f,	0.71f,	0.64f,	0.0f,	1.0f,	-1.0f,
		0.5f,	0.0f,  0.5f,	0.59f,	0.35f,	0.0f,	1.0f,	-1.0f,

		// 6
		//x		y		z		S		T
		0.5f,	0.0f,  0.5f,	0.76f,	0.64f,	-1.0f,	1.0f,	0.0f,
		0.0f,	-0.75f,  0.0f,	0.85f,	0.35f,	-1.0f,	1.0f,	0.0f,
		0.5f,	0.0f,  -0.5f,	0.98f,	0.64f,	-1.0f,	1.0f,	0.0f,

		// 7 
		//x		y		z		S		T
		0.5f,	0.0f,  -0.5f,	0.68f,	0.68f,	0.0f,	1.0f,	1.0f,
		0.0f,	-0.75f,  0.0f,	0.57f,	0.98f,	0.0f,	1.0f,	1.0f,
		-0.5f,	0.0f,  -0.5f,	0.46f,	0.68f,	0.0f,	1.0f,	1.0f,

		// 8
		//x		y		z		S		T
		-0.5f,	0.0f,  -0.5,	0.32f,	0.32f,	1.0f,	1.0f,	0.0f,
		0.0f,	-0.75f,  0.0f,	0.42f,	0.05f,	1.0f,	1.0f,	0.0f,
		-0.5f,	0.0f,  0.5f,	0.55f,	0.32f,	1.0f,	1.0f,	0.0f
	};


	Mesh* octaedro = new Mesh();
	octaedro->CreateMesh(octaedro_vertices, octaedro_indices, 192, 24);
	meshList.push_back(octaedro);

}


void CreateShaders()
{
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}



int main()
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	CrearDado();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 0.3f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	pisoTexture = Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	AgaveTexture = Texture("Textures/Agave.tga");
	AgaveTexture.LoadTextureA();

	Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt_optimizado.obj");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/llanta_optimizada.obj");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	octaedro = Texture("Textures/Octaedro.png");
	octaedro.LoadTextureA();

	Luminaria = Model();
	Luminaria.LoadModel("Models/luminaria.obj");

	Carro = Model();
	Cofre = Model();
	Llanta = Model();

	Carro.LoadModel("Models/carro.obj");
	Cofre.LoadModel("Models/cofre.obj");
	Llanta.LoadModel("Models/llanta.obj");


	std::vector<std::string> skyboxFaces;
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");

	skybox = Skybox(skyboxFaces);

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);


	//luz direccional, s�lo 1 y siempre debe de existir 
	// Para la luz del sol
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
		0.3f, 0.3f,
		0.0f, 0.0f, -1.0f);
	//contador de luces puntuales
	unsigned int pointLightCount = 0;

	//Declaraci�n de primer luz puntual
	pointLights[0] = PointLight(1.0f, 1.0f, 1.0f,
		1.0f, 1.0f,
		5.0f, 3.0f, -4.0f,
		0.3f, 0.2f, 0.1f);
	pointLightCount++;

	unsigned int spotLightCount = 0;
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		5.0f);
	spotLightCount++;

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	GLuint uniformColor = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 1000.0f);

	glm::mat4 model(1.0);
	glm::mat4 modelaux(1.0);
	glm::mat4 centroCarro(1.0);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

	////Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		deltaTime += (now - lastTime) / limitFPS;
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();
		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformColor = shaderList[0].getColorLocation();

		//informaci�n en el shader de intensidad especular y brillo
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		// luz ligada a la c�mara de tipo flash
		//sirve para que en tiempo de ejecuci�n (dentro del while) se cambien propiedades de la luz
		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		////Luz Carro
		//glm::vec3 luzCarro = glm::vec3(0.0f, -0.29f, -5.0 + (mainWindow.getAvanzarCarro() + mainWindow.getRetrocederCarro()) * 0.05);
		//glm::vec3 lightDirCarro = glm::vec3(0.0f, 0.0f, 1.0f);
		//spotLights[0].SetFlash(luzCarro, lightDirCarro);

		////Luz Helicoptero
		//glm::vec3 luzHelicoptero = glm::vec3(0.0 + ((mainWindow.getAvanzarHeli() + mainWindow.getRetrocederHeli()) * 0.05), 5.0f, -12.0f);
		//glm::vec3 lightDirHeli = glm::vec3(0.0f, -1.0f, 0.0f);
		//spotLights[1].SetFlash(luzHelicoptero, lightDirHeli);

		//informaci�n al shader de fuentes de iluminaci�n
		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);

		if (mainWindow.getLuminaria() == 1) {
			shaderList[0].SetPointLights(pointLights, pointLightCount);
		}else if (mainWindow.getLuminaria() == 0){
			shaderList[0].SetPointLights(pointLights, pointLightCount-1);
		}

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(30.0f, 1.0f, 30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniform3fv(uniformColor, 1, glm::value_ptr(color));

		pisoTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);

		meshList[2]->RenderMesh();

		////Instancia del coche
		//model = glm::mat4(1.0);

		//// Carro		
		//model = glm::translate(model, glm::vec3(0.0f, -0.29f, -5.0f));
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, (mainWindow.getAvanzarCarro() * 0.05)));
		//model = glm::translate(model, glm::vec3(0.0f, 0.0f, (mainWindow.getRetrocederCarro() * 0.05)));

		//modelaux = model;

		//color = glm::vec3(0.8706f, 0.7804f, 0.5569f);
		//glUniform3fv(uniformColor, 1, glm::value_ptr(color));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Carro.RenderModel();

		//model = modelaux;

		//centroCarro = model;

		////Llanta Delantera Izquierda
		//model = glm::translate(model, glm::vec3(0.9f, -0.3f, 1.55f));

		//modelaux = model;

		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(0 - mainWindow.getAvanzarCarro()), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(0 - mainWindow.getRetrocederCarro()), glm::vec3(1.0f, 0.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Llanta.RenderModel();

		//model = centroCarro;

		////Llanta Delantera Derecha
		//model = glm::translate(model, glm::vec3(-0.9f, -0.3f, 1.55f));

		//modelaux = model;

		//model = glm::rotate(model, glm::radians(mainWindow.getAvanzarCarro()), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getRetrocederCarro()), glm::vec3(1.0f, 0.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Llanta.RenderModel();

		//model = centroCarro;

		////Llanta Trasera Izquierda
		//model = glm::translate(model, glm::vec3(0.9f, -0.3f, -1.55f));

		//modelaux = model;

		//model = glm::rotate(model, 180 * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(0 - mainWindow.getAvanzarCarro()), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(0 - mainWindow.getRetrocederCarro()), glm::vec3(1.0f, 0.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Llanta.RenderModel();

		//model = centroCarro;

		////Llanta Trasera Derecha
		//model = glm::translate(model, glm::vec3(-0.9f, -0.3f, -1.55f));

		//modelaux = model;

		//model = glm::rotate(model, glm::radians(mainWindow.getAvanzarCarro()), glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getRetrocederCarro()), glm::vec3(1.0f, 0.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Llanta.RenderModel();

		//model = centroCarro;

		////Cofre
		//model = glm::translate(model, glm::vec3(0.0f, 0.3f, 1.3f));

		//modelaux = model;

		//model = glm::rotate(model, glm::radians(mainWindow.getCofre()), glm::vec3(1.0f, 0.0f, 0.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Cofre.RenderModel();

		//model = modelaux;

		////Helic�ptero
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, 5.0f, -12.0));
		//model = glm::translate(model, glm::vec3((mainWindow.getAvanzarHeli() * 0.05), 0.0f, 0.0f));
		//model = glm::translate(model, glm::vec3((mainWindow.getRetrocederHeli() * 0.05), 0.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.3f, 0.3f, 0.3f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));

		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Blackhawk_M.RenderModel();

		//Luminaria
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(5.0f, -1.0f, -4.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Luminaria.RenderModel();

		////Agave �qu� sucede si lo renderizan antes del coche y el helic�ptero?
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, 1.0f, -4.0f));
		//model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		////blending: transparencia o traslucidez
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//AgaveTexture.UseTexture();
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[3]->RenderMesh();
		//glDisable(GL_BLEND);

		//Octaedro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, 4.5f, -2.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		octaedro.UseTexture();
		meshList[4]->RenderMesh();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}
