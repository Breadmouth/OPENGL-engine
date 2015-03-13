#include "GPUParticleEmitter.h"
#include "Renderer.h"

GPUParticleEmitter::GPUParticleEmitter()
		: m_particles(nullptr), m_maxParticles(0),
		m_position(0, 0, 0),
		m_drawShader(0),
		m_updateShader(0),
		m_lastDrawTime(0)
{
	m_vao[0] = 0;
	m_vao[1] = 0;
	m_vbo[0] = 0;
	m_vbo[1] = 0;
}

GPUParticleEmitter::~GPUParticleEmitter()
{
	delete[] m_particles;

	glDeleteVertexArrays(2, m_vao);
	glDeleteBuffers(2, m_vbo);

	glDeleteProgram(m_drawShader);
	glDeleteProgram(m_updateShader);
}

void GPUParticleEmitter::Init(unsigned int a_maxParticles,
	float a_lifetimeMin, float a_lifetimeMax, float a_velocityMin,
	float a_velocityMax, float a_startSize, float a_endSize,
	const glm::vec4& a_startColour, const glm::vec4& a_endColour)
{
	m_startColour = a_startColour;
	m_endColour = a_endColour;
	m_startSize = a_startSize;
	m_endSize = a_endSize;
	m_velocityMin = a_velocityMin;
	m_velocityMax = a_velocityMax;
	m_lifespanMin = a_lifetimeMin;
	m_lifespanMax = a_lifetimeMax;
	m_maxParticles = a_maxParticles;
	m_particles = new GPUParticle[a_maxParticles];

	// set our starting ping-pong buffer
	m_activeBuffer = 0;

	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();
}

void GPUParticleEmitter::CreateBuffers()
{
	glGenVertexArrays(2, m_vao);
	glGenBuffers(2, m_vbo);

	// setup the first buffer
	glBindVertexArray(m_vao[0]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles *
		sizeof(GPUParticle), m_particles, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // velocity
	glEnableVertexAttribArray(2); // lifetime
	glEnableVertexAttribArray(3); // lifespan

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), ((char*)0) + 28);
	glBindVertexArray(m_vao[1]);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_maxParticles * sizeof(GPUParticle), 0, GL_STREAM_DRAW);

	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // velocity
	glEnableVertexAttribArray(2); // lifetime
	glEnableVertexAttribArray(3); // lifespan

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), 0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), ((char*)0) + 12);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), ((char*)0) + 24);
	glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE,
		sizeof(GPUParticle), ((char*)0) + 28);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GPUParticleEmitter::CreateDrawShader()
{
	unsigned int vs = Renderer::LoadShaderFromFile("../data/shaders/gpuParticleShader.glvs", GL_VERTEX_SHADER);
	unsigned int gs = Renderer::LoadShaderFromFile("../data/shaders/gpuParticleShader.glgs", GL_GEOMETRY_SHADER);
	unsigned int fs = Renderer::LoadShaderFromFile("../data/shaders/gpuParticleShader.glfs", GL_FRAGMENT_SHADER);

	m_drawShader = glCreateProgram();
	glAttachShader(m_drawShader, vs);
	glAttachShader(m_drawShader, fs);
	glAttachShader(m_drawShader, gs);
	glLinkProgram(m_drawShader);

	glDeleteShader(vs);
	glDeleteShader(gs);
	glDeleteShader(fs);

	glUseProgram(m_drawShader);

	int location = glGetUniformLocation(m_drawShader, "sizeStart");
	glUniform1f(location, m_startSize);
	location = glGetUniformLocation(m_drawShader, "sizeEnd");
	glUniform1f(location, m_endSize);

	location = glGetUniformLocation(m_drawShader, "colourStart");
	glUniform4fv(location, 1, &m_startColour[0]);
	location = glGetUniformLocation(m_drawShader, "colourEnd");
	glUniform4fv(location, 1, &m_endColour[0]);
}

void GPUParticleEmitter::CreateUpdateShader()
{
	unsigned int vs = Renderer::LoadShaderFromFile("../data/shaders/gpuParticleUpdate.glvs", GL_VERTEX_SHADER);

	m_updateShader = glCreateProgram();
	glAttachShader(m_updateShader, vs);

	//specify the data we will stream back
	const char* varyings[] = { "position", "velocity", "lifetime", "lifespan" };
	glTransformFeedbackVaryings(m_updateShader, 4, varyings, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(m_updateShader);

	glDeleteShader(vs);

	glUseProgram(m_updateShader);

	int location = glGetUniformLocation(m_updateShader, "lifeMin");
	glUniform1f(location, m_lifespanMin);

	location = glGetUniformLocation(m_updateShader, "lifeMax");
	glUniform1f(location, m_lifespanMax);
}

void GPUParticleEmitter::Draw(float time, const glm::mat4& a_cameraTransform, const glm::mat4& a_projectionView)
{
	//update particles using transform feedback
	glUseProgram(m_updateShader);

	int location = glGetUniformLocation(m_updateShader, "time");
	glUniform1f(location, time);

	float deltaTime = time - m_lastDrawTime;
	m_lastDrawTime = time;

	location = glGetUniformLocation(m_updateShader, "deltaTime");
	glUniform1f(location, deltaTime);

	location = glGetUniformLocation(m_updateShader, "emitterPosition");
	glUniform3fv(location, 1, &m_position[0]);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_vao[m_activeBuffer]);

	unsigned int otherBuffer = (m_activeBuffer + 1) % 2;

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbo[otherBuffer]);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);	// draw the particles using the Geometry SHader to billboard them
	glUseProgram(m_drawShader);

	location = glGetUniformLocation(m_drawShader, "projectionView");
	glUniformMatrix4fv(location, 1, false, &a_projectionView[0][0]);

	location = glGetUniformLocation(m_drawShader, "cameraTransform");
	glUniformMatrix4fv(location, 1, false, &a_cameraTransform[0][0]);

	// draw particles in the "other" buffer
	glBindVertexArray(m_vao[otherBuffer]);
	glDrawArrays(GL_POINTS, 0, m_maxParticles);

	// swap for next frame
	m_activeBuffer = otherBuffer;
}