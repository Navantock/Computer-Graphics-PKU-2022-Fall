#include <GL/glew.h>
#include <glm/glm.hpp>
#include <stb_image.h>
#include <vector>
#include <random>
#include <cmath>
#include <string>

#include "shader.h"
#include "config.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

string particleTexPath = "particles.jpg";

typedef struct Particle{
	bool active;
	glm::vec3 position, velocity;
	glm::vec2 tex_coordinates;
	glm::vec4 color;
	GLfloat size;
	GLfloat lifetime;

	Particle() : active(false), position(0.0f), velocity(0.0f), tex_coordinates(0.0f), color(1.0f), size(1.0f), lifetime(0.0f) {}
	Particle(float _life) : active(true), position(0.0f), velocity(0.0f), tex_coordinates(0.0f), color(1.0f), size(1.0f), lifetime(_life) {}
};


class ParticleSystem {
public:
	unsigned int texture;
	GLuint VAO;
	GLuint particle_num;
	GLfloat particle_size;
	float life;
	glm::vec3 color_rgb;
	std::vector<Particle> particles;
	bool enable_alpha;
	bool enableTexture;
	bool useColorTrans;

	ParticleSystem(GLuint nums, GLfloat _size, float _life);
	~ParticleSystem();
	void UpdateState(float dt);
	void Draw(Shader shader);
	void RenderInit();
	void SetTex(const string& _path);
	virtual void StyleInit() = 0; //override to generate specifc particle style
	virtual void SpawnParticle(GLuint index);
	virtual void UpdatePos(GLuint index, float dt);
	virtual void UpdateV(GLuint index, float dt);
	virtual void UpdateColor(GLuint index, float dt);
	virtual void UpdateTex(GLuint index, float dt);
	virtual void UpdateStyle(); // call it before updating particles, override to update specific members
};

ParticleSystem::ParticleSystem(GLuint _nums, GLfloat _size = 1.0f, float _life = 10.0f) :
	enableTexture(false), particle_num(_nums), particle_size(_size), life(_life), color_rgb(glm::vec3(1.0, 1.0, 1.0)),
	useColorTrans(false), enable_alpha(false)
{
	for (GLuint i = 0; i < _nums; ++i) {
		this->particles.push_back(Particle(_life));
	}

	this->RenderInit();
}

ParticleSystem::~ParticleSystem() {
	delete this;
}

void ParticleSystem::SetTex(const string& _path) {
	glGenTextures(1, &this->texture);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nrChannels;
	unsigned char* data = stbi_load(_path.c_str(), &width, &height, &nrChannels, 0);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load particle texture" << std::endl;
	}
	stbi_image_free(data);
}

void ParticleSystem::RenderInit() {
	GLuint VBO;
	glGenVertexArrays(1, &this->VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	if (enableTexture) {
		GLfloat vertices[] = {
		this->particle_size / 2, this->particle_size / 2, this->particle_size / 2, .1f, -.1f, .1f, 0.0f, 0.0f,
		-this->particle_size / 2, -this->particle_size / 2, this->particle_size / 2, .1f, -.1f, .1f, 0.0f, 1.0f,
		this->particle_size / 2, -this->particle_size / 2, -this->particle_size / 2, .1f, -.1f, .1f, 1.0f, 1.0f,

		this->particle_size / 2, this->particle_size / 2, this->particle_size / 2, -.1f, .1f, .1f, 0.0f, 0.0f,
		-this->particle_size / 2, -this->particle_size / 2, this->particle_size / 2, -.1f, .1f, .1f, 0.0f, 1.0f,
		-this->particle_size / 2, this->particle_size / 2, -this->particle_size / 2, -.1f, .1f, .1f, 1.0f, 1.0f,

		this->particle_size / 2, this->particle_size / 2, this->particle_size / 2, .1f, .1f, -.1f, 0.0f, 0.0f,
		this->particle_size / 2, -this->particle_size / 2, -this->particle_size / 2, .1f, .1f, -.1f, 0.0f, 1.0f,
		-this->particle_size / 2, this->particle_size / 2, -this->particle_size / 2, .1f, .1f, -.1f, 1.0f, 1.0f,

		-this->particle_size / 2, -this->particle_size / 2, this->particle_size / 2, -.1f, -.1f, -.1f, 0.0f, 0.0f,
		this->particle_size / 2, -this->particle_size / 2, -this->particle_size / 2, -.1f, -.1f, -.1f, 0.0f, 1.0f,
		-this->particle_size / 2, this->particle_size / 2, -this->particle_size / 2, -.1f, -.1f, -.1f, 1.0f, 1.0f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	}
	else {
		GLfloat vertices[] = {
		this->particle_size / 2, this->particle_size / 2, this->particle_size / 2, .1f, -.1f, .1f,
		-this->particle_size / 2, -this->particle_size / 2, this->particle_size / 2, .1f, -.1f, .1f,
		this->particle_size / 2, -this->particle_size / 2, -this->particle_size / 2, .1f, -.1f, .1f,

		this->particle_size / 2, this->particle_size / 2, this->particle_size / 2, -.1f, .1f, .1f,
		-this->particle_size / 2, -this->particle_size / 2, this->particle_size / 2, -.1f, .1f, .1f, 
		-this->particle_size / 2, this->particle_size / 2, -this->particle_size / 2, -.1f, .1f, .1f, 

		this->particle_size / 2, this->particle_size / 2, this->particle_size / 2, .1f, .1f, -.1f,
		this->particle_size / 2, -this->particle_size / 2, -this->particle_size / 2, .1f, .1f, -.1f,
		-this->particle_size / 2, this->particle_size / 2, -this->particle_size / 2, .1f, .1f, -.1f,

		-this->particle_size / 2, -this->particle_size / 2, this->particle_size / 2, -.1f, -.1f, -.1f,
		this->particle_size / 2, -this->particle_size / 2, -this->particle_size / 2, -.1f, -.1f, -.1f,
		-this->particle_size / 2, this->particle_size / 2, -this->particle_size / 2, -.1f, -.1f, -.1f
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	}
}

void ParticleSystem::SpawnParticle(GLuint index) {
	this->particles[index] = Particle(this->life);
}

void ParticleSystem::UpdateState(float dt) {
	UpdateStyle();
	for (GLuint i = 0; i < this->particle_num; ++i) {
		this->particles[i].lifetime -= dt;
		if (this->particles[i].lifetime < 0) {
			this->SpawnParticle(i);
			continue;
		}
		this->UpdatePos(i, dt);
		this->UpdateV(i, dt);
		if(this->enableTexture)
			this->UpdateTex(i, dt);
		else
			this->UpdateColor(i, dt);
	}
}

void ParticleSystem::Draw(Shader shader) {
	this->RenderInit();
	if (this->enable_alpha) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	}
	for (Particle particle : this->particles)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, particle.position);
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		shader.setMat4("model", model);
		if (particle.lifetime > 0.0f) {
			shader.setVec3("particlePos", particle.position);
			if (this->enableTexture) {
				glBindTexture(GL_TEXTURE_2D, this->texture);
				shader.setBool("enable_particle_tex", true);
			}
			else {
				shader.setBool("enable_particle_tex", false);
				shader.setVec4("particle_color", particle.color);
			}
			glBindVertexArray(this->VAO);
			glDrawArrays(GL_TRIANGLES, 0, 12);
			glBindVertexArray(3);
		}
	}
	if (this->enable_alpha)
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystem::UpdatePos(GLuint index, float dt) {
	this->particles[index].position += dt * this->particles[index].velocity;
}

void ParticleSystem::UpdateV(GLuint index, float dt) {}
void ParticleSystem::UpdateColor(GLuint index, float dt) {}
void ParticleSystem::UpdateTex(GLuint index, float dt) {}
void ParticleSystem::UpdateStyle() {}


class ParticleRing : public ParticleSystem {
public:
	float radius;
	float var;
	float latitude;
	float angular_velocity;
	glm::vec3 normal;
	std::normal_distribution<float> gauss_distribution;
	std::uniform_real_distribution<float> uniform_distribution;
	std::random_device rd;
	std::default_random_engine gen {rd()};

	ParticleRing(GLuint nums, float _life, GLfloat _size, float _radius, float _var, float _latitude, float _angle_v, glm::vec3 _normal);
private:
	void StyleInit();
	void SpawnParticle(GLuint index);
	void UpdatePos(GLuint index, float dt);
	void UpdateV(GLuint index, float dt);
	void UpdateColor(GLuint index, float dt);
	void UpdateTex(GLuint index, float dt);
	void UpdateStyle();
};

ParticleRing::ParticleRing(GLuint nums, float _life, GLfloat _size, float _radius = 10.0f, float _var = 0.5f, float _latitude = 0.0f, float _angle_v = M_PI / 36, glm::vec3 _normal = glm::normalize(glm::vec3(.0f, 1.0f, .0f))) :
	ParticleSystem(nums, _size, _life), radius(_radius), var(_var), latitude(_latitude), angular_velocity(_angle_v), normal(glm::normalize(_normal)), gauss_distribution(_radius, _var), uniform_distribution(0, 1)
{
	this->StyleInit();
};

void ParticleRing::StyleInit() {
	for (GLuint i = 0; i < this->particle_num; ++i) {
		SpawnParticle(i);
		this->particles[i].lifetime = this->uniform_distribution(this->gen) * this->life;
	}
}

void ParticleRing::SpawnParticle(GLuint index) {
	//sampling
	float r = this->gauss_distribution(this->gen);
	float angle = this->uniform_distribution(this->gen) * 2 * M_PI;
	//activate particle
	this->particles[index].active = true;
	//Give lifetime
	this->particles[index].lifetime = this->life;
	//calculate position by coordinate system rotation
	glm::vec3 axis = glm::cross(this->normal, glm::vec3(0.0f, 1.0f, 0.0f));
	float cosTheta = this->normal.y;
	glm::qua<float> q = glm::quat(sqrt((1 + cosTheta) / 2), axis.x * sqrt((1 - cosTheta) / 2), axis.y * sqrt((1 - cosTheta) / 2), axis.z * sqrt((1 - cosTheta) / 2));
	glm::vec3 r0 = r * glm::normalize(q * glm::vec3(cos(angle), 0.0f, sin(angle)));
	this->particles[index].position = r0 + this->latitude * this->normal;

	//There's no need to define velocity in circular motion. If you still want to do this, remember to use modified velocity with reference below
	/*axis = this->normal;
	cosTheta = this->angular_velocity;
	q = glm::quat(sqrt((1 + cosTheta) / 2), axis.x * sqrt((1 - cosTheta) / 2), axis.y * sqrt((1 - cosTheta) / 2), axis.z * sqrt((1 - cosTheta) / 2));
	this->particles[index].velocity = q * r0 - r0;*/

	//Generate color
	this->UpdateColor(index, 0.0f);
}

void ParticleRing::UpdatePos(GLuint index, float dt) {
	float cosTheta = cos(dt * this->angular_velocity);
	glm::qua<float> q = glm::quat(sqrt((1 + cosTheta) / 2), this->normal.x * sqrt((1 - cosTheta) / 2), this->normal.y * sqrt((1 - cosTheta) / 2), this->normal.z * sqrt((1 - cosTheta) / 2));
	this->particles[index].position = q * this->particles[index].position;
}

void ParticleRing::UpdateV(GLuint index, float dt) {}

void ParticleRing::UpdateColor(GLuint index, float dt) {
	float interAlpha = this->particles[index].lifetime / this->life;
	if (this->useColorTrans) {
		float interRgb = min(abs(glm::length(this->particles[index].position - this->normal) - this->radius) / (3 * this->var), 1.0f);
		this->particles[index].color = glm::vec4(1.0f - interRgb * (1.0f - this->color_rgb), interAlpha);
	}
	else {
		this->particles[index].color = glm::vec4(this->color_rgb, interAlpha);
	}
}

void ParticleRing::UpdateTex(GLuint index, float dt) {
	float interAlpha = this->particles[index].lifetime / this->life;
	this->particles[index].color.w = interAlpha;
}

void ParticleRing::UpdateStyle() {
	if (this->radius <= 0)
		this->radius = 0.0001f;
	if (this->var <= 0)
		this->var = 0.0001f;
	this->gauss_distribution = std::normal_distribution<float>(this->radius, this->var);
}