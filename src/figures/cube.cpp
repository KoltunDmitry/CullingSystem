#include "Cube.hpp"
#include <algorithm>

static const GLfloat DEFAULT_CUBE[COUNT_VERTEX_CUBE] = {
			-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f,
			1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,
			1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
			1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f,
			1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
			1.0f, 1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
			1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, 1.0f
};

static const GLfloat DEFAULT_CUBE_COLOR[COUNT_VERTEX_CUBE] = {
	  0.583f, 0.771f, 0.014f, 0.609f, 0.115f, 0.436f, 0.327f, 0.483f, 0.844f,
	  0.822f, 0.569f, 0.201f, 0.435f, 0.602f, 0.223f, 0.310f, 0.747f, 0.185f,
	  0.597f, 0.770f, 0.761f, 0.559f, 0.436f, 0.730f, 0.359f, 0.583f, 0.152f,
	  0.483f, 0.596f, 0.789f, 0.559f, 0.861f, 0.639f, 0.195f, 0.548f, 0.859f,
	  0.014f, 0.184f, 0.576f, 0.771f, 0.328f, 0.970f, 0.406f, 0.615f, 0.116f,
	  0.676f, 0.977f, 0.133f, 0.971f, 0.572f, 0.833f, 0.140f, 0.616f, 0.489f,
	  0.997f, 0.513f, 0.064f, 0.945f, 0.719f, 0.592f, 0.543f, 0.021f, 0.978f,
	  0.279f, 0.317f, 0.505f, 0.167f, 0.620f, 0.077f, 0.347f, 0.857f, 0.137f,
	  0.055f, 0.953f, 0.042f, 0.714f, 0.505f, 0.345f, 0.783f, 0.290f, 0.734f,
	  0.722f, 0.645f, 0.174f, 0.302f, 0.455f, 0.848f, 0.225f, 0.587f, 0.040f,
	  0.517f, 0.713f, 0.338f, 0.053f, 0.959f, 0.120f, 0.393f, 0.621f, 0.362f,
	  0.673f, 0.211f, 0.457f, 0.820f, 0.883f, 0.371f, 0.982f, 0.099f, 0.879f };

Cube::Cube(std::shared_ptr<glm::mat4> viewProjection, GLuint programID)
	: Drawable()
	, m_viewProjection(viewProjection)
	, m_programID(programID)
	, m_worldTransform(glm::mat4(1.0f))
{
	m_matrixID = glGetUniformLocation(m_programID, "MVP");

	for (int i = 0; i < COUNT_VERTEX_CUBE; i++)
	{
		m_vertexBufferData[i] = DEFAULT_CUBE[i];
		m_vertexColorData[i] = DEFAULT_CUBE_COLOR[i];
	}

	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexBufferData),
		m_vertexBufferData, GL_STATIC_DRAW);

	glGenBuffers(1, &m_colorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertexColorData),
		m_vertexColorData, GL_STATIC_DRAW);

	computeAABB();
}

void Cube::setWorldTransform(glm::mat4 matrix)
{
	m_worldTransform = matrix;
	computeAABB();
	computeSphere();
}

void Cube::draw(float dt)
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glm::mat4 MVP = *m_viewProjection.get() * m_worldTransform;
	glUniformMatrix4fv(m_matrixID, 1, GL_FALSE, &MVP[0][0]);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glVertexAttribPointer(0,  // attribute. No particular reason for 0, but must
							  // match the layout in the shader.
		3,         // size
		GL_FLOAT,  // type
		GL_FALSE,  // normalized?
		0,         // stride
		(void*)0   // array buffer offset
	);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, m_colorBuffer);
	glVertexAttribPointer(1,  // attribute. No particular reason for 1, but must
							  // match the layout in the shader.
		3,         // size
		GL_FLOAT,  // type
		GL_FALSE,  // normalized?
		0,         // stride
		(void*)0   // array buffer offset
	);

	// Draw the triangle !
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3);  // 12*3 indices starting at 0 -> 12 triangles

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Cube::computeAABB()
{
	for(int i = 0; i < COUNT_VERTEX_CUBE/3; i++)
	{
		glm::vec3 temp = m_worldTransform * glm::vec4(m_vertexBufferData[i*3], m_vertexBufferData[i*3 + 1],
			m_vertexBufferData[i*3 + 2], 1);
		if(i == 0)
		{
			m_aabb.min = temp;
			m_aabb.max = temp;
		}
		else
		{
			if(temp.x < m_aabb.min.x)
			{
				m_aabb.min.x = temp.x;
			}
			if (temp.y < m_aabb.min.y)
			{
				m_aabb.min.y = temp.y;
			}
			if (temp.z < m_aabb.min.z)
			{
				m_aabb.min.z = temp.z;
			}
			if (temp.x > m_aabb.max.x)
			{
				m_aabb.max.x = temp.x;
			}
			if (temp.y > m_aabb.max.y)
			{
				m_aabb.max.y = temp.y;
			}
			if (temp.z > m_aabb.max.z)
			{
				m_aabb.max.z = temp.z;
			}
		}
	}
}

void Cube::computeSphere()
{
	//make transform for all vertices
	std::vector<glm::vec3> transformedVertices;
	transformedVertices.reserve(COUNT_VERTEX_CUBE / 3);


	for (int i = 0; i < COUNT_VERTEX_CUBE / 3; i++)
	{
		glm::vec3 temp = m_worldTransform * glm::vec4(m_vertexBufferData[i * 3], m_vertexBufferData[i * 3 + 1],
			m_vertexBufferData[i * 3 + 2], 1);
		transformedVertices.push_back(temp);
	}

	auto distance = [](glm::vec3 a, glm::vec3 b) -> float {
		return sqrt((a.x - b.x)*(a.x - b.x) + (a.y - b.y)*(a.y - b.y) + (a.z - b.z)*(a.z - b.z));
	};

	glm::vec3 a = transformedVertices.front();
	glm::vec3 b = a;
	float max = distance(a, b);

	for(auto& v : transformedVertices)
	{
		if(distance(a,v) > max)
		{
			b = v;
			max = distance(a, v);
		}

	}
	//b is new
	glm::vec3 c = b;
	max = distance(b, c);

	for (auto& v : transformedVertices)
	{
		if (distance(b, v) > max)
		{
			c = v;
			max = distance(b, v);
		}
	}

	m_sphere.center = (b + c);
	m_sphere.center /= 2;

	m_sphere.radius = distance(b, c) / 2;
}
