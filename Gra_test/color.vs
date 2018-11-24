//
// color.vs : Code of the vertex shader in HSLS language.
//
// The vertex shader is called by the GPU when it is processing data from the vertex buffers
// that have been sent to it
//
// In shader programs you begin with the global variables. 
// These globals can be modified externally from the C++ code.
// You can use many types of variables such as int or float
// and then set them externally for the shader program to use.
// Generally you will put most globals in buffer object types called "cbuffer"
// even if it is just a single global variable.
// Logically organizing these buffers is important for efficient execution of shaders
// as well as how the graphics card will store the buffers.
//
// Definition of types that have x, y, z, w position vectors
// and red, green, blue, alpha colors.
// The POSITION, COLOR, and SV_POSITION are semantics that convey to the GPU the use of the variable.
//

// Globals
cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// Typedefs
// POSITION works for vertex shaders and SV_POSITION works for pixel shaders while COLOR works for both
// If you want more than one of the same type then you have to add a number to the end such as COLOR0, COLOR1, and so forth.
struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// The input to the vertex shader must match the data format in the vertex buffer
// as well as the type definition in the shader source file which in this case is VertexInputType.
// The output of the vertex shader will be sent to the pixel shader.
// In this case the output type is called PixelInputType which is defined above as well.
// It takes the position of the input vertex and multiplies it by the world, view, and then projection matrices.
// This will place the vertex in the correct location for rendering in 3D space
// according to our view and then onto the 2D screen.
// After that the output variable takes a copy of the input color and then returns the output
// which will be used as input to the pixel shader.
// Also note that I do set the W value of the input position to 1.0
// otherwise it is undefined since we only read in a XYZ vector for position.

// Vertex shader
PixelInputType ColorVertexShader(VertexInputType input)
{
    PixelInputType output;
    
    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the input color for the pixel shader to use.
    output.color = input.color;
    
    return output;
}
