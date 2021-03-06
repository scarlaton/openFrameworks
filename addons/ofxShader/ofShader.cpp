#include "ofShader.h"


char *textFileRead(const char *fn);		// some memory allocation happens here
										// be careful...  please don't call load shader 
										// repeatedly !!!!! (you have been warned)



//---------------------------------------------------------------
void ofShader::loadShader(const char * fragmentName, const char * vertexName){
	
	
	bLoaded = false;	
	
	if (GLEE_ARB_shader_objects){
		
		// ---------------------------------- (a) load in the shaders
		char *vs = NULL,*fs = NULL;
		vs = textFileRead(vertexName);
		fs = textFileRead(fragmentName);
		
		vertexShader = (GLhandleARB)glCreateShader(GL_VERTEX_SHADER);
		fragmentShader = (GLhandleARB)glCreateShader(GL_FRAGMENT_SHADER);
		
		GLint length = strlen(vs);
		glShaderSourceARB(vertexShader, 1, (const char**)&vs, &length);
		length = strlen(fs);
		glShaderSourceARB(fragmentShader, 1, (const char**)&fs, &length);
		
		char infobuffer[1000];
		GLsizei infobufferlen = 0;
		
		// ----------------------------------- (b) compile the shaders
		// try to compile "vertex shader"
		glCompileShaderARB(vertexShader);
		
		//please add compile status check in:
		GLint compileStatus = 0;
		glGetObjectParameterivARB( vertexShader, GL_COMPILE_STATUS, &compileStatus );
		if(compileStatus > 0) {
			printf("vertexShader      [%s] compiled \n",vertexName);
		} else if (compileStatus == 1) {
			printf("!! vertexShader   [%s] compilation issues, status : %i !!\n", vertexName, compileStatus );
		}
		
		
		glGetInfoLogARB(vertexShader, 999, &infobufferlen, infobuffer);
		if (infobufferlen != 0){
			infobuffer[infobufferlen] = 0;
			printf("vertexShader [%s] reports: %s \n", vertexName, infobuffer);
			return;
		}
		
		// ------------------------------------
		// try to compile "fragment shader"
		glCompileShaderARB(fragmentShader);
		
		

		glGetObjectParameterivARB( fragmentShader, GL_COMPILE_STATUS, &compileStatus );
		if(compileStatus > 0) {
			printf("fragmentShader    [%s] compiled \n",fragmentName);
		} else if (compileStatus == 1) {
			printf("!! fragmentShader [%s] compilation issues, status : %i !!\n", fragmentName, compileStatus );
		}		
		
		glGetInfoLogARB(fragmentShader, 999, &infobufferlen, infobuffer);
		if (infobufferlen != 0){
			infobuffer[infobufferlen] = 0;
			printf("fragmentShader [%s] reports: %s \n", fragmentName, infobuffer);
			return;
		}
		
		// ----------------------------------- (c) link the shaders
		shader = glCreateProgramObjectARB();
		glAttachObjectARB(shader,vertexShader);
		glAttachObjectARB(shader,fragmentShader);
		glLinkProgramARB(shader);
		
		// texture on unit 0
		tex0 = NULL;
		
		bLoaded = true;
			
	} else {
	
		printf("sorry, it looks like you can't run 'ARB_shader_objects' \n please check the capabilites of your graphics card (http://www.ozone3d.net/gpu_caps_viewer/) \n");
	
	}


}
		
//---------------------------------------------------------------
void ofShader::loadShader(const char * shaderName, bool hasPath){
	bLoaded = false;
	char fragmentName[1024];
	char vertexName[1024];
	
	if (hasPath)
	{
		sprintf(fragmentName,"%s.frag", shaderName);
		sprintf(vertexName, "%s.vert", shaderName);
	}
	else
	{
		char path[255];
		strcpy(path, ofToDataPath(shaderName).c_str()); //this adds the right path to the data folder no matter what OS 
		sprintf(fragmentName,"%s.frag", path);
		sprintf(vertexName, "%s.vert", path);
	}

	//printf("ROGER: frag=%s\n",fragmentName);
	//printf("ROGER vert=%s\n",vertexName);
	loadShader(fragmentName, vertexName);
	
	strcpy(name, shaderName);
}

//---------------------------------------------------------------
// ROGER
void ofShader::loadShaderFV(const char * frag, const char * vert){
	bLoaded = false;
	char fragmentName[1024];
	char vertexName[1024];
	
	sprintf(fragmentName,"%s.frag", ofToDataPath(frag).c_str());
	sprintf(vertexName, "%s.vert", ofToDataPath(vert).c_str());
	loadShader(fragmentName, vertexName);
	
	strcpy(name, frag);
}



//---------------------------------------------------------------
void ofShader::unload(){
	
	if (vertexShader){
		glDetachObjectARB(shader,vertexShader);
		glDeleteObjectARB(vertexShader);
		vertexShader = NULL;
	}
	
	if (fragmentShader){
		glDetachObjectARB(shader,fragmentShader);
		glDeleteObjectARB(fragmentShader);
		fragmentShader = NULL;
	}
	
	if (shader){
		glDeleteObjectARB(shader);
		shader = NULL;
	}
	
}

//---------------------------------------------------------------
void ofShader::setShaderActive(bool bActive){
	if (GLEE_ARB_shader_objects && bLoaded == true){
		if (bActive == true){
			glUseProgramObjectARB(shader);
		} else {
			glUseProgramObjectARB(0);
		}
	}
}

// Apply Shader FX to the screen
//---------------------------------------------------------------
void ofShader::apply(){
	if (tex0 == NULL)
	{
		printf("ofshader:: [%s] texture unit 0 sampler not set!\n",name);
		return;
	}
	// Draw quad
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);
	glDisable(GL_COLOR_MATERIAL);
	ofSetColor(255,255,255,255);
	tex0->draw(0.0, 0.0);
}

// Apply Shader FX to a FBO
// BUT... It is faster to render the screen and then call myTex.loadScreenData(0, 0, ofGetWidth(), ofGetHeight());
//---------------------------------------------------------------
void ofShader::apply(ofxFBOTexture *fbo){
	fbo->begin();
	this->apply();
	fbo->end();
}


/*
//________________________.
//							\__________________.
// FLOAT										\__________________.
//																	\___________________.
//______________________________________________________________________________________\__________________.
*/
void ofShader::setUniform ( const char * name, float v1) {
	if(bLoaded ){
		glUniform1fARB(glGetUniformLocationARB(shader, name), v1);
	}
}

void ofShader::setUniform ( const char * name, float v1, float v2){
	if (bLoaded ){
		glUniform2fARB(glGetUniformLocationARB(shader, name), v1, v2);
	}
}

void ofShader::setUniform ( const char * name, float v1, float v2, float v3){
	if (bLoaded	){
		glUniform3fARB(glGetUniformLocationARB(shader, name), v1, v2, v3);
	}
}

void ofShader::setUniform ( const char * name, float v1, float v2, float v3, float v4){
	if (bLoaded ){
		glUniform4fARB(glGetUniformLocationARB(shader, name), v1, v2, v3, v4);
	}
}

// vec2 usage: setUniform ("name", (float*)v, 2);
// vec3 usage: setUniform ("name", (float*)v, 3);
// vec4 usage: setUniform ("name", (float*)v, 4);
void ofShader::setUniform ( const char * name, float *v, int count){
	if (count == 2)
		glUniform2fvARB(glGetUniformLocationARB(shader, name), count, (GLfloat*)v);
	else if (count == 3)
		glUniform3fvARB(glGetUniformLocationARB(shader, name), count, (GLfloat*)v);
	else if (count == 4)
		glUniform4fvARB(glGetUniformLocationARB(shader, name), count, (GLfloat*)v);
	else
		printf("ofShader::setUniform: Wrong size [%d]. Please use 2 (vec2), 3 (vec3) or 4 (vec4).\n",count);
}

/*
//_________________________.
//							\__________________.
// INT											\__________________.
//																	\___________________.
//_______________________________________________________________________________________\__________________.
*/
void ofShader::setUniform ( const char * name, int v1){
	if (bLoaded ){
		glUniform1iARB(glGetUniformLocationARB(shader, name), v1);
	}
}

void ofShader::setUniform ( const char * name, int v1, int v2){
	if (bLoaded ){
		glUniform2iARB(glGetUniformLocationARB(shader, name), v1, v2);
	}
}

void ofShader::setUniform ( const char * name, int v1, int v2, int v3){
	if (bLoaded ){
		glUniform3iARB(glGetUniformLocationARB(shader, name), v1, v2, v3);
	}
}

void ofShader::setUniform ( const char * name, int v1, int v2, int v3, int v4){
	if (bLoaded ){
		glUniform4iARB(glGetUniformLocationARB(shader, name), v1, v2, v3, v4);
	}
}

// ivec2 usage: setUniform ("name", (long*)v, 2);
// ivec3 usage: setUniform ("name", (long*)v, 3);
// ivec4 usage: setUniform ("name", (long*)v, 4);
// OBS: GLint is in fact a long
void ofShader::setUniform ( const char * name, long *v, int count){
	if (count == 2)
		glUniform2ivARB(glGetUniformLocationARB(shader, name), count, (GLint*)v);
	else if (count == 3)
		glUniform3ivARB(glGetUniformLocationARB(shader, name), count, (GLint*)v);
	else if (count == 4)
		glUniform4ivARB(glGetUniformLocationARB(shader, name), count, (GLint*)v);
	else
		printf("ofShader::setUniform: Wrong size [%d]. Please use 2 (ivec2), 3 (ivec3) or 4 (ivec4).\n",count);
}


/*
 //_________________________.
 //							\__________________.
 // SAMPLERS									\__________________.
 //																	\___________________.
 //_______________________________________________________________________________________\__________________.
 */

void ofShader::setSampler ( const char * name, int unit, ofTexture *tex)
{
	glActiveTexture(GL_TEXTURE0+unit);									// move to desired unit
	glBindTexture(tex->texData.textureTarget, tex->texData.textureID);	// bind to unit
	glActiveTexture(GL_TEXTURE0);										// move back to default unit
	this->setUniform(name, (int)unit);									// send unit to shader as int
	// save texture on unit 0 for apply()
	if (unit == 0)
		tex0 = tex;
}


/*
 //________________________.
 //							\__________________.
 // FLOAT		ATTRIB							\__________________.
 //																	\___________________.
 //______________________________________________________________________________________\__________________.
 */
void ofShader::setAttribute ( const char * name, float v1) {
	if(bLoaded ){
		glVertexAttrib1f(glGetAttribLocation((GLuint)shader, name), v1);
	}
}

void ofShader::setAttribute ( const char * name, float v1, float v2){
	if (bLoaded ){
		glVertexAttrib2f(glGetAttribLocation((GLuint)shader, name), v1, v2);
	}
}

void ofShader::setAttribute ( const char * name, float v1, float v2, float v3){
	if (bLoaded	){
		glVertexAttrib3f(glGetAttribLocation((GLuint)shader, name), v1, v2, v3);
	}
}

void ofShader::setAttribute ( const char * name, float v1, float v2, float v3, float v4){
	if (bLoaded ){
		glVertexAttrib4f(glGetAttribLocation((GLuint)shader, name), v1, v2, v3, v4);
	}
}

/*
 //_________________________.
 //							\__________________.
 // Short				ATTRIB					\__________________.
 //																	\___________________.
 //_______________________________________________________________________________________\__________________.
 */
void ofShader::setAttribute ( const char * name, short v1){
	if (bLoaded ){
		glVertexAttrib1s(glGetAttribLocation((GLuint)shader, name), v1);
	}
}

void ofShader::setAttribute ( const char * name, short v1, short v2){
	if (bLoaded ){
		glVertexAttrib2s(glGetAttribLocation((GLuint)shader, name), v1, v2);
	}
}

void ofShader::setAttribute ( const char * name, short v1, short v2, short v3){
	if (bLoaded ){
		glVertexAttrib3s(glGetAttribLocation((GLuint)shader, name), v1, v2, v3);
	}
}

void ofShader::setAttribute ( const char * name, short v1, short v2, short v3, short v4){
	if (bLoaded ){
		glVertexAttrib4s(glGetAttribLocation((GLuint)shader, name), v1, v2, v3, v4);
	}
}

/*
 //_________________________.
 //							\__________________.
 // DOUBLE				ATTRIB					\__________________.
 //																	\___________________.
 //_______________________________________________________________________________________\__________________.
 */
void ofShader::setAttribute ( const char * name, double v1){
	if (bLoaded ){
		glVertexAttrib1s(glGetAttribLocation((GLuint)shader, name), v1);
	}
}

void ofShader::setAttribute ( const char * name, double v1, double v2){
	if (bLoaded ){
		glVertexAttrib2s(glGetAttribLocation((GLuint)shader, name), v1, v2);
	}
}

void ofShader::setAttribute ( const char * name, double v1, double v2, double v3){
	if (bLoaded ){
		glVertexAttrib3s(glGetAttribLocation((GLuint)shader, name), v1, v2, v3);
	}
}

void ofShader::setAttribute ( const char * name, double v1, double v2, double v3, double v4){
	if (bLoaded ){
		glVertexAttrib4s(glGetAttribLocation((GLuint)shader, name), v1, v2, v3, v4);
	}
}


/* getting the shit from attribs. standard C is:

void glGetVertexAttribfv(	GLuint  	index,
							GLenum  	pname,
							GLfloat * 	params);

let's try to accomodate it.

Getting Attrib location:

GLint glGetAttribLocation(	GLuint  	program,
							const GLchar * 	name);
*/

void ofShader::getActiveVertexAttribute(const char * name, GLfloat * returnValue) {
	GLint loc = glGetAttribLocation((GLuint) shader, name);
	//GLint numAttributes = 0;
	//glGetProgramiv( (GLuint)shader, GL_ACTIVE_ATTRIBUTES, &numAttributes );
	//printf("numAttributes: %i\n", numAttributes);
	
	//GLint attributeMaxLength = 0;
	//glGetProgramiv( (GLuint)shader, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attributeMaxLength );
	//printf("attributeMaxLength: %i\n", attributeMaxLength);
	
	//GLint count = -1;
	//GLenum type = 0;
	//GLchar* attributeName = new GLchar[attributeMaxLength];
	//for ( GLint i = 0; i < numAttributes; ++i )
	//{
		//GLsizei length;
		//glGetActiveAttrib( (GLuint)shader, loc-1, attributeMaxLength, &length, &count, &type, returnValue); 
		//glGetVertexAttribfv(loc, GL_CURRENT_VERTEX_ATTRIB, returnValue);
		
		/*for(int j = 0; j < length; j++) {
			printf("%c", attributeName[j]);
		}
		printf("\n");
	}*/
	printf("----------------------\n");
	//delete [] attributeName;
}
	


/*//----------------------------------------------------------------------------------------------------------
*/
void ofShader::printActiveUniforms() {
	//GLint location = glGetUniformLocation((GLuint)shader, name);
	// now find it in the list of uniforms to get type and size
	GLint numUniforms = 0;
	glGetProgramiv( (GLuint)shader, GL_ACTIVE_UNIFORMS, &numUniforms );
	printf("numUniforms: %i\n", numUniforms);
	
	GLint uniformMaxLength = 0;
	glGetProgramiv( (GLuint)shader, GL_ACTIVE_UNIFORM_MAX_LENGTH, &uniformMaxLength );
	printf("uniformMaxLength: %i\n", uniformMaxLength);
	
	GLint count = -1;
	GLenum type = 0;
	GLchar* uniformName = new GLchar[uniformMaxLength];
	for ( GLint i = 0; i < numUniforms; ++i )
	{
		GLsizei length;
		glGetActiveUniform( (GLuint)shader, i, uniformMaxLength, &length, &count, &type, uniformName); 
	
		for(int j = 0; j < length; j++) {
			printf("%c", uniformName[j]);
		}
		printf(" @ index %i\n", glGetUniformLocation((GLuint)shader, uniformName));
	}
	printf("----------------------\n");
	delete [] uniformName;
}

void ofShader::printActiveAttributes() {
	//GLint location = glGetUniformLocation((GLuint)shader, name);
	// now find it in the list of uniforms to get type and size
	GLint numAttributes = 0;
	glGetProgramiv( (GLuint)shader, GL_ACTIVE_ATTRIBUTES, &numAttributes );
	printf("numAttributes: %i\n", numAttributes);
	
	GLint attributeMaxLength = 0;
	glGetProgramiv( (GLuint)shader, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &attributeMaxLength );
	printf("attributeMaxLength: %i\n", attributeMaxLength);
	
	GLint count = -1;
	GLenum type = 0;
	GLchar* attributeName = new GLchar[attributeMaxLength];
	for ( GLint i = 0; i < numAttributes; ++i )
	{
		GLsizei length;
		glGetActiveAttrib( (GLuint)shader, i, attributeMaxLength, &length, &count, &type, attributeName); 
		
		for(int j = 0; j < length; j++) {
			printf("%c", attributeName[j]);
		}
		printf(" @ index %i\n", glGetAttribLocation((GLuint)shader, attributeName));
	}
	printf("----------------------\n");
	delete [] attributeName;
}


//---------------------------------------------------------
// below is from: www.lighthouse3d.com
// you may use these functions freely. they are provided as is, and no warranties, either implicit, or explicit are given
//---------------------------------------------------------

char *textFileRead(const char *fn) {

	FILE *fp;
	char *content 	= 	NULL;
	int count		=	0;

	if (fn != NULL) {
		fp = fopen(fn,"rt");
		if (fp != NULL) {
		
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	
	return content;
}
