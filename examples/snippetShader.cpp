/// [ShaderAttach example]
Shader shader;

shader.attach("./shader/shader.vert");
shader.attach("./shader/shader.frag").
shader.attach("./shader/shader.geom");
/// [ShaderAttach example]
/// [ShaderAttach exampleChaining]
Shader shader;

shader.attach("./shader/shader.vert").attach("./shader/shader.frag").attach("./shader/shader.geom");
/// [ShaderAttach exampleChaining]



/// [ShaderLink example]
Shader shader;

shader.attach("./shader/shader.vert");
shader.attach("./shader/shader.frag").
shader.attach("./shader/shader.geom");

shader.link();
/// [ShaderLink example]
/// [ShaderLink exampleChaining]
Shader shader;

shader.attach("./shader/shader.vert").attach("./shader/shader.frag").attach("./shader/shader.geom").link();
/// [ShaderLink exampleChaining]

/// [ShaderRecompile example]
Shader shader;

shader.attach("./shader/shader.vert");
shader.attach("./shader/shader.frag").
shader.attach("./shader/shader.geom");
shader.link();


//Modify your shader source code


try {
    shader.recompile();
} catch (Shader::LinkException const &e) {
    // Do some stuff
    std::cerr << e.what() << std::endl;
}  catch (Shader::CreateException const &e) {
    // Do some stuff
    std::cerr << e.what() << std::endl;
}

/// [ShaderRecompile example]

/// [ShaderActivate example]
Shader shader;

shader.attach("./shader/shader.vert");
shader.attach("./shader/shader.frag").
shader.attach("./shader/shader.geom");
shader.link();

shader.activate();
shader.setInt("uNumber", 42); //Set uniform to the Activated Shader

/// [ShaderActivate example]

/// [ShaderSetUniform example]

vec3 position(0.0f, 0.0f, 0.0f);
shader.activate();
shader.setVec3("uNumber", position); //Set uniform to the Activated Shader

/// [ShaderSetUniform example]