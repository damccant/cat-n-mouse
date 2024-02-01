#include "shader.h"

inline static const char* nameForType(const unsigned int type)
{
	switch(type)
	{
	case GL_VERTEX_SHADER: return "Vertex";
	case GL_FRAGMENT_SHADER: return "Fragment";
	default: return "Unknown";
	}
}

/**
 * Creates a shader of the specified type from the given source and compiles it
 */
static unsigned int createShader(unsigned int type, const char* src, const size_t size)
{
	printf(nameForType(type));
	printf(" Shader (%zu bytes):\n", size);
	for(size_t i = 0; i < size; i++)
		putc(src[i], stdout);


	unsigned int shader = glCreateShader(type);
	const int size_i = size;
	glShaderSource(shader, 1, &src, &size_i);
	glCompileShader(shader);

	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "Error: " << nameForType(type) << " Shader compilation failed!\n" << infoLog << std::endl;
	}

	return shader;
}

// include the compiled in shaders

extern const char* _binary_vertexShader_vert_start;
extern const char* _binary_vertexShader_vert_end;
// the linker seems to not like this
//extern const size_t _binary_vertexShader_vert_size;
const size_t _binary_vertexShader_vert_size = _binary_vertexShader_vert_end - _binary_vertexShader_vert_start;

static unsigned int createVertexShader(void)
{
	return createShader(GL_VERTEX_SHADER, _binary_vertexShader_vert_start, _binary_vertexShader_vert_size);
}

extern const char* _binary_fragmentShader_frag_start;
extern const char* _binary_fragmentShader_frag_end;
// the linker seems to not like this
//extern const size_t _binary_fragmentShader_frag_size;
const size_t _binary_fragmentShader_frag_size = _binary_fragmentShader_frag_end - _binary_fragmentShader_frag_start;

static unsigned int createFragmentShader(void)
{
	return createShader(GL_FRAGMENT_SHADER, _binary_fragmentShader_frag_start, _binary_fragmentShader_frag_size);
}

inline static unsigned int createShaderProgram(void)
{
	unsigned int vertexShader = createVertexShader();
	unsigned int fragmentShader = createFragmentShader();
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "Error: Shader program compilation failed!\n" << infoLog << std::endl;
	}

	// now that the program is linked, the shaders are no longer needed
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return shaderProgram;
}

Shader::Shader()
{
	id = createShaderProgram();
}

Shader::~Shader()
{
	// delete the now unneeded shader program
	glDeleteProgram(id);
}

void Shader::use()
{
	glUseProgram(id);
}