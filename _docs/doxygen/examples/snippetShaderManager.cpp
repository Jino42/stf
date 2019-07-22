/// [ShaderManagerAddShader example]
ShaderManager::Get().addShader("VoxelShader");
ShaderManager::Get().addShader("DebugNormal");
/// [ShaderManagerAddShader example]

/// [ShaderManagerGetShader example]
ShaderManager::Get().getShader("VoxelShader").attach("./shader/voxel.vert").attach("./shader/voxel.frag");
ShaderManager::Get().getShader("VoxelShader").link();

Shader &shader = ShaderManager::Get().getShader("DebugNormal");
shader.attach("./debug.vert").attach("./debug.geom").attach("./debug.frag").link();
/// [ShaderManagerGetShader example]

/// [ShaderManagerReload example]
ShaderManager::Get().reload();
/// [ShaderManagerReload example]