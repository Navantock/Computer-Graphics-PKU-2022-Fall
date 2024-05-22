#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

#include "model.h"
#include "shader.h"

const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

class LightSrc {
public:
	glm::vec3 lightPos;
	glm::vec3 lightColor;
	glm::mat4 shadowProj;
	std::vector<glm::mat4> shadowTransforms;

	LightSrc(glm::vec3 _lightpos) : lightPos(_lightpos), lightColor(glm::vec3(1.0f, 1.0f, 1.0f)) { this->setShadow(); }
	LightSrc(glm::vec3 _lightpos, glm::vec3 _lightcolor, string const& _path) : lightPos(_lightpos), lightColor(_lightcolor){  }
	void use(Shader shader);
	void use2(Shader shader);
private:
	GLuint depthMapFBO;
	GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
	GLfloat near = 1.0f;
	GLfloat far = 25.0f;
	void setShadow();
};

void LightSrc::use(Shader shader) {
	this->shadowProj = glm::perspective(glm::radians(90.0f), this->aspect, this->near, this->far);

	this->shadowTransforms.push_back(this->shadowProj * glm::lookAt(this->lightPos, this->lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	this->shadowTransforms.push_back(this->shadowProj * glm::lookAt(this->lightPos, this->lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
	this->shadowTransforms.push_back(this->shadowProj * glm::lookAt(this->lightPos, this->lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
	this->shadowTransforms.push_back(this->shadowProj * glm::lookAt(this->lightPos, this->lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
	this->shadowTransforms.push_back(this->shadowProj * glm::lookAt(this->lightPos, this->lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
	this->shadowTransforms.push_back(this->shadowProj * glm::lookAt(this->lightPos, this->lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	shader.use();
	for (GLuint i = 0; i < 6; ++i)
		shader.setMat4(("shadowMatrices[" + std::to_string(i) + "]").c_str(), this->shadowTransforms[i]);
	this->use2(shader);
}

void LightSrc::use2(Shader shader) {
	shader.setFloat("far_plane", this->far);
	shader.setVec3("lightPos", this->lightPos);
	shader.setVec3("lightColor", this->lightColor);
}

void LightSrc::setShadow() {
	glGenFramebuffers(1, &this->depthMapFBO);
	GLuint depthCubemap;
	glGenTextures(1, &depthCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
	for (GLuint i = 0; i < 6; ++i)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
			SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindFramebuffer(GL_FRAMEBUFFER, this->depthMapFBO);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}
