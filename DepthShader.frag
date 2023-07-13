#version 330 core
out vec4 FragColor;

float near = 0.1;
float far = 5.0;
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}
void main()
{             
     //gl_FragDepth = gl_FragCoord.z;
     float Depth = LinearizeDepth(gl_FragCoord.z);
     FragColor = vec4(vec3(Depth),1.0);
     //FragColor = vec4(1.0,0.0,0.0,1.0);
}