#define GLEW_STATIC
#include <Gl/glew.h>
#define GLFW_DILL
#include <GLFW/glfw3.h>
#include <stdio.h>

int main() {
    printf("hello\n");

    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);



    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1024, 768, "WINDOW", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glewExperimental = GL_TRUE;
    glfwMakeContextCurrent(window);



    GLenum ret = glewInit();
    if (GLEW_OK != glewInit())
    {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(ret));
        return 1;

    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    const char* version_str = glGetString(GL_VERSION);
    const char* Device_str = glGetString(GL_RENDERER);
    printf("The OpenGL version is running in %s\n", version_str);
    printf("The OpenGL version is running in %s\n", Device_str);

    //hárömszöggeci
    float triangle_Verticles[] = {
        0.0f, 0.5f, 0.0f,  //top
        0.5f, -0.5f, 0.0f, // right
        -0.5f, -0.5f, 0.0f, // left

    };
    GLuint vertex_buffer_object = 0;
    glGenBuffers(1,&vertex_buffer_object);
    {


        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object); // bind

        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * 3, triangle_Verticles,GL_STATIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0); // unbind
    }
    GLuint vertex_array_object = 0;
    glGenVertexArrays(1, &vertex_array_object);
    { 
        glBindVertexArray(vertex_array_object); // bind

        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0); // unbind
    }


    GLint shader_program_object = 0;
    {
        const char* vertex_shader_str =
            "#version 410\n"
            "uniform float u_time_s;\n"
            "in vec3 a_Position;\n"
            "void main() {"
            "  gl_Position = vec4(a_Position, 1.0);"
            "  gl_Position.y += sin( u_time_s );"
            "}\n";
 

    
        const char* fragment_shader_str =
           "#version 410\n"
           "uniform float u_time_s;\n"
           "out vec4 frag_colour;\n"
           "void main() {"
            "  frag_colour = vec4( sin(u_time_s * 2.3), sin(u_time_s), cos(u_time_s), 1.0);"
           "}\n";
  


    GLuint vertex_shader_object = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader_object, 1, &vertex_shader_str, NULL);
    glCompileShader(vertex_shader_object);
    GLuint fragment_shader_object = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader_object, 1, &fragment_shader_str, NULL);
    glCompileShader(fragment_shader_object);

    shader_program_object = glCreateProgram();

    glBindAttribLocation(shader_program_object, 0, "a_Position"); // The index passed into glBindAttribLocation is
    glAttachShader(shader_program_object, vertex_shader_object);
    glAttachShader(shader_program_object, fragment_shader_object);
    glLinkProgram(shader_program_object);

    };
    GLint u_time_loc = glGetUniformLocation(shader_program_object, "u_time_s");

    glClearColor(0.6f, 0.6f, 0.8f, 1.0f);
    while (!glfwWindowShouldClose(window))
    {
        double curr_s = glfwGetTime();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shader_program_object);
        {
            glUniform1f(u_time_loc, curr_s);
            glBindVertexArray(vertex_array_object);
            {
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }
            glBindVertexArray(0);
        }
        glUseProgram(0);


        glfwPollEvents();
        glfwSwapBuffers(window);
    };

    glfwTerminate();
	return 0;


} 