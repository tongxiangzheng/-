#include"shell.h"
#include<windows.h>
int main(){
	readTXT();
	PlaySound(L"bkMusic.wav",nullptr,SND_FILENAME|SND_ASYNC|SND_LOOP);
	if(init()==-1){
		return -1;
	}
	for(int i=0;i<0;i++){
		add_food();
	}
	setmode(4);
	shaderProgram1=initShader("Shaders/vertexShader.gl","Shaders/fragmentShader_food.gl");
	shaderProgram2=initShader("Shaders/vertexShader.gl","Shaders/fragmentShader_snake.gl");
	shaderProgram3=initShader("Shaders/vertexShader.gl","Shaders/fragmentShader_background.gl");
	shaderProgram4=initShader("Shaders/vertexShader.gl","Shaders/fragmentShader_type.gl");
	VAO=initVAO();
	vertexUniformColor1 = glGetUniformLocation(shaderProgram1, "Color");
	vertexUniformPosDelta = glGetUniformLocation(shaderProgram1, "PosDelta");
	vertexUniformR = glGetUniformLocation(shaderProgram1, "R");
	vertexUniformColor4 = glGetUniformLocation(shaderProgram4, "Color");
	vertexUniformText= glGetUniformLocation(shaderProgram4, "text");
	initFont();
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
		display();
		sync();
        glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram1);
	glDeleteProgram(shaderProgram2);
	glDeleteProgram(shaderProgram3);
	glDeleteProgram(shaderProgram4);
    glfwTerminate();
    return 0;
}