#include "boid.hpp"
#include <array>
#include <filesystem>

float Boid::s_AlignmentMult_ = 1.f;
float Boid::s_CohesionMult_ = 1.f;
float Boid::s_SeparationMult_ = 1.f;
float Boid::s_Perception_ = 30.f;
float Boid::s_MaxForce_ = 10.f;
float Boid::s_MaxVel_ = 30.f;
unsigned int Boid::s_EBOSize_ = 0;
GLuint Boid::s_VAO_ = 0;
GLuint Boid::s_VBO_ = 0;
GLuint Boid::s_EBO_ = 0;
GLuint Boid::s_Shader_ = 0;

GLint Boid::s_ModelLocation_ = 0;
GLint Boid::s_ViewLocation_ = 0;
GLint Boid::s_ProjLocation_ = 0;
GLint Boid::s_CamPositionLocation_ = 0;
GLint Boid::s_UseDirLightLocation_ = 0;
GLint Boid::s_UsePointLightLocation_ = 0;
GLint Boid::s_UseSpotLightLocation_ = 0;

Boid::Material Boid::s_Material_;

Boid::Boid(const glm::vec3& pos, const glm::vec3& vel) : 
    m_Pos_(pos),
    m_Vel_(vel),
    m_Acc_(0.f, 0.f, 0.f)
{
    calcModelMatrix();
}

Boid::~Boid() {
    
}

void Boid::calcModelMatrix() {
    auto geometryFront = glm::vec3(0.0f, 0.f, 1.f);
    auto normVel = glm::normalize(m_Vel_);
    auto rotationVec = glm::cross(geometryFront, normVel);
    auto angle = glm::acos(glm::dot(geometryFront, normVel));

    m_Model_ = glm::mat4(1.f);
    m_Model_ = glm::translate(m_Model_, m_Pos_);
    if (glm::length2(rotationVec) != 0.f) {
        m_Model_ = glm::rotate(m_Model_, angle, rotationVec);
    }
    m_Model_ = glm::scale(m_Model_, glm::vec3(7.f));
}

void Boid::show(const Camera& camera, Space& space, bool useDirLight, bool usePointLight, bool useSpotLight) {
    abcg::glUseProgram(s_Shader_);
    abcg::glBindVertexArray(s_VAO_);

    //activate and bind the texture
    abcg::glActiveTexture(GL_TEXTURE0);
    abcg::glBindTexture(GL_TEXTURE_2D, s_Material_.diffuseTexture);
    abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    abcg::glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //set the material uniform
    abcg::glUniform1i(s_Material_.LocationDiffuseTexture, 0);
    abcg::glUniform3fv(s_Material_.LocationAmbient, 1, &s_Material_.ambient[0]);
    abcg::glUniform3fv(s_Material_.LocationDiffuse, 1, &s_Material_.diffuse[0]);
    abcg::glUniform3fv(s_Material_.LocationSpecular, 1, &s_Material_.specular[0]);
    abcg::glUniform1fv(s_Material_.LocationShininess, 1, &s_Material_.shininess);

    //set the directional light uniform
    int useDirLightI = useDirLight;
    abcg::glUniform1i(s_UseDirLightLocation_, useDirLightI);
    auto& dirLight = space.getDirLight();
    abcg::glUniform3fv(dirLight.LocationDirection, 1, &dirLight.direction[0]);
    abcg::glUniform3fv(dirLight.LocationAmbient, 1, &dirLight.ambient[0]);
    abcg::glUniform3fv(dirLight.LocationDiffuse, 1, &dirLight.diffuse[0]);
    abcg::glUniform3fv(dirLight.LocationSpecular, 1, &dirLight.specular[0]);
    abcg::glUniform1f(dirLight.LocationStrength, dirLight.strength);

    int usePointLightI = usePointLight;
    abcg::glUniform1i(s_UsePointLightLocation_, usePointLightI);
    //set the point lights uniforms
    for (auto& pointLight : space.getPointLights())
    {
        abcg::glUniform3fv(pointLight.LocationPosition, 1, &pointLight.position[0]);
        abcg::glUniform1f(pointLight.LocationConstant, pointLight.constant);
        abcg::glUniform1f(pointLight.LocationLinear, pointLight.linear);
        abcg::glUniform1f(pointLight.LocationQuadractic, pointLight.quadratic);
        abcg::glUniform3fv(pointLight.LocationAmbient, 1, &pointLight.ambient[0]);
        abcg::glUniform3fv(pointLight.LocationDiffuse, 1, &pointLight.diffuse[0]);
        abcg::glUniform3fv(pointLight.LocationSpecular, 1, &pointLight.specular[0]);
    }

    int usSpotLightI = useSpotLight;
    abcg::glUniform1i(s_UseSpotLightLocation_, usSpotLightI);
    //set the spot lights uniforms
    for (auto& spotLight : space.getSpotLights())
    {
        abcg::glUniform3fv(spotLight.LocationPosition, 1, &spotLight.position[0]);
        abcg::glUniform3fv(spotLight.LocationDirection, 1, &spotLight.direction[0]);
        abcg::glUniform1f(spotLight.LocationConstant, spotLight.constant);
        abcg::glUniform1f(spotLight.LocationLinear, spotLight.linear);
        abcg::glUniform1f(spotLight.LocationQuadractic, spotLight.quadratic);
        abcg::glUniform1f(spotLight.LocationCutOff, spotLight.cutOff);
        abcg::glUniform1f(spotLight.LocationOuterCutOff, spotLight.outerCutOff);
        abcg::glUniform3fv(spotLight.LocationAmbient, 1, &spotLight.ambient[0]);
        abcg::glUniform3fv(spotLight.LocationDiffuse, 1, &spotLight.diffuse[0]);
        abcg::glUniform3fv(spotLight.LocationSpecular, 1, &spotLight.specular[0]);
    }

    //set the camera uniform
    abcg::glUniform3fv(s_CamPositionLocation_, 1, &camera.getPosition()[0]);

    //set mvp uniform
    abcg::glUniformMatrix4fv(s_ModelLocation_, 1, GL_FALSE, &m_Model_[0][0]);
    abcg::glUniformMatrix4fv(s_ViewLocation_, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);
    abcg::glUniformMatrix4fv(s_ProjLocation_, 1, GL_FALSE, &camera.getProjMatrix()[0][0]);

    //draw
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO_);
    abcg::glDrawElements(GL_TRIANGLES, s_EBOSize_, GL_UNSIGNED_INT, nullptr);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    abcg::glBindVertexArray(0);
    abcg::glUseProgram(0);
}

void Boid::checkEdge(float worldSize) {
    //Check if the current position is outside the
    //box and reposition it

    if (m_Pos_.x > worldSize) {
        m_Pos_.x = -worldSize;
    } else if (m_Pos_.x < -worldSize) {
        m_Pos_.x = worldSize;
    }

    if (m_Pos_.y > worldSize) {
        m_Pos_.y = -worldSize;
    } else if (m_Pos_.y < -worldSize) {
        m_Pos_.y = worldSize;
    }

    if (m_Pos_.z > worldSize) {
        m_Pos_.z = -worldSize;
    } else if (m_Pos_.z < -worldSize) {
        m_Pos_.z = worldSize;
    }
}

glm::vec3 Boid::steer(const glm::vec3& desired) {
    //Make the magnitude the vector equal to s_MaxVel_
    glm::vec3 steerVec = glm::normalize(desired) * s_MaxVel_;
    //Steer toward the disired target
    steerVec -= m_Vel_;
    //Clamp the vector so it doesn't go above s_MaxForce_
    if (glm::length(steerVec) > s_MaxForce_) {
        steerVec = glm::normalize(steerVec) * s_MaxForce_;
    }
    return steerVec;
}

void Boid::simulate(const std::vector<Boid>& boids) {
    //The simulation consists of three parts:
    //  Alignment: Avg velocity of neighbour boids
    //  Cohesion: Avg position of neighbour boids
    //  Separation: Avoid collision (inversely proportional to distance)

    m_Acc_ = glm::vec3(0.f);
    glm::vec3 avgAlignment(0.f);
    glm::vec3 avgCohesion(0.f);
    glm::vec3 avgSeparation(0.f);

    //Loop over all the boids and collect the desired information
    //from those who are inside the sphere of perception radius
    //and are different from the current boid
    unsigned int total = 0;
    for (const auto& b : boids) {
        float d = glm::distance(m_Pos_, b.m_Pos_);
        if (&b != this && d < s_Perception_) {
            avgAlignment += b.m_Vel_;
            avgCohesion += b.m_Pos_;
            avgSeparation += (m_Pos_ - b.m_Pos_)/(d*d);
            total++;
        }
    }

    if (!total)
        return;

    //Takes the average
    avgAlignment /= (float)total;
    avgCohesion /= (float)total;
    avgSeparation /= (float)total;

    //Cohesion depends on the current boid position
    avgCohesion -= m_Pos_;

    //Steer towards each part of the simulation and accumulate in the acceleration
    m_Acc_ += s_AlignmentMult_ * steer(avgAlignment) + s_CohesionMult_ * steer(avgCohesion) + s_SeparationMult_ * steer(avgSeparation);
}

void Boid::update(float dt) {
    //Update the position and velocity
    m_Pos_ += dt * m_Vel_;
    calcModelMatrix();

    m_Vel_ += dt * m_Acc_;
    
    //Clamp the velocity to s_MaxVel_
    if (glm::length(m_Vel_) > s_MaxVel_) {
        m_Vel_ = glm::normalize(m_Vel_) * s_MaxVel_;
    }

    //Reset the acceleration
    m_Acc_ = glm::vec3(0.f);
}

void Boid::setup(Space& space) {
    const auto assetsPath = abcg::Application::getAssetsPath();
    auto [vertices, indices] = loadModelFromFile(assetsPath + "eagle/eagle.obj", true);

    s_EBOSize_ = indices.size();

	s_Shader_ = abcg::createOpenGLProgram({
		{.source = assetsPath + "boid.vert", .stage = abcg::ShaderStage::Vertex},
		{.source = assetsPath + "boid.frag", .stage = abcg::ShaderStage::Fragment}
	});
    //get the attribute locations
    auto postionLoc = abcg::glGetAttribLocation(s_Shader_, "l_Position");
    auto normalLoc = abcg::glGetAttribLocation(s_Shader_, "l_Normal");
    auto texLoc = abcg::glGetAttribLocation(s_Shader_, "l_Tex");

    //get uniform locations
    s_ModelLocation_ = abcg::glGetUniformLocation(s_Shader_, "u_Model");
    s_ViewLocation_ = abcg::glGetUniformLocation(s_Shader_, "u_View");
    s_ProjLocation_ = abcg::glGetUniformLocation(s_Shader_, "u_Proj");
    s_CamPositionLocation_ = abcg::glGetUniformLocation(s_Shader_, "u_CamPosition");
    s_UseDirLightLocation_ = abcg::glGetUniformLocation(s_Shader_, "u_UseDirLight");
    s_UsePointLightLocation_ = abcg::glGetUniformLocation(s_Shader_, "u_UsePointLight");
    s_UseSpotLightLocation_ = abcg::glGetUniformLocation(s_Shader_, "u_UseSpotLight");

    s_Material_.LocationDiffuseTexture = abcg::glGetUniformLocation(s_Shader_, "u_Material.diffuseTexture");
    s_Material_.LocationAmbient = abcg::glGetUniformLocation(s_Shader_, "u_Material.ambient");
    s_Material_.LocationDiffuse = abcg::glGetUniformLocation(s_Shader_, "u_Material.diffuse");
    s_Material_.LocationSpecular = abcg::glGetUniformLocation(s_Shader_, "u_Material.specular");
    s_Material_.LocationShininess = abcg::glGetUniformLocation(s_Shader_, "u_Material.shininess");

    auto& dirLight = space.getDirLight();
    dirLight.LocationDirection = abcg::glGetUniformLocation(s_Shader_, "u_DirLight.direction");
    dirLight.LocationAmbient = abcg::glGetUniformLocation(s_Shader_, "u_DirLight.ambient");
    dirLight.LocationDiffuse = abcg::glGetUniformLocation(s_Shader_, "u_DirLight.diffuse");
    dirLight.LocationSpecular = abcg::glGetUniformLocation(s_Shader_, "u_DirLight.specular");
    dirLight.LocationStrength = abcg::glGetUniformLocation(s_Shader_, "u_DirLight.strength");

    auto& pointLights = space.getPointLights();
    for (size_t i = 0; i < pointLights.size(); i++) {
        auto prefix = "u_PointLights[" + std::to_string(i) + "].";
        auto& pointLight = pointLights[i];

        auto name = prefix + "position";
        pointLight.LocationPosition = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "constant";
        pointLight.LocationConstant = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "linear";
        pointLight.LocationLinear = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "quadratic";
        pointLight.LocationQuadractic = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "ambient";
        pointLight.LocationAmbient = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "diffuse";
        pointLight.LocationDiffuse = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "specular";
        pointLight.LocationSpecular = abcg::glGetUniformLocation(s_Shader_, name.c_str());
    }

    auto& spotLights = space.getSpotLights();
    for (size_t i = 0; i < spotLights.size(); i++)
    {
        auto prefix = "u_SpotLights[" + std::to_string(i) + "].";
        auto& spotLight = spotLights[i];

        auto name = prefix + "position";
        spotLight.LocationPosition = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "direction";
        spotLight.LocationDirection = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "constant";
        spotLight.LocationConstant = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "linear";
        spotLight.LocationLinear = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "quadratic";
        spotLight.LocationQuadractic = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "cutOff";
        spotLight.LocationCutOff = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "outerCutOff";
        spotLight.LocationOuterCutOff = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "ambient";
        spotLight.LocationAmbient = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "diffuse";
        spotLight.LocationDiffuse = abcg::glGetUniformLocation(s_Shader_, name.c_str());
        name = prefix + "specular";
        spotLight.LocationSpecular = abcg::glGetUniformLocation(s_Shader_, name.c_str());
    }

    //gen the vao and buffers
    abcg::glGenBuffers(1, &s_VBO_);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, s_VBO_);
    abcg::glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);

    abcg::glGenBuffers(1, &s_EBO_);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, s_EBO_);
    abcg::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    abcg::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    abcg::glGenVertexArrays(1, &s_VAO_);
    abcg::glBindVertexArray(s_VAO_);
    abcg::glBindBuffer(GL_ARRAY_BUFFER, s_VBO_);

    abcg::glEnableVertexAttribArray(postionLoc);
    abcg::glVertexAttribPointer(postionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    abcg::glEnableVertexAttribArray(normalLoc);
    abcg::glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    abcg::glEnableVertexAttribArray(texLoc);
    abcg::glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    abcg::glBindBuffer(GL_ARRAY_BUFFER, 0);
    abcg::glBindVertexArray(0);
}

void Boid::showUI(int id) {
    if (!ImGui::CollapsingHeader("Boids"))
        return;
    ImGui::DragFloat("Aligment", &s_AlignmentMult_, 0.1f, 0.f, 10.f);
    ImGui::DragFloat("Cohesion", &s_CohesionMult_, 0.1f, 0.f, 10.f);
    ImGui::DragFloat("Separation", &s_SeparationMult_, 0.1f, 0.f, 10.f);
    ImGui::DragFloat("Perception Radius", &s_Perception_, 0.1f, 0.f, 10.f);
    ImGui::DragFloat("Maximum Force", &s_MaxForce_);
    ImGui::DragFloat("Velocity", &s_MaxVel_);
    s_Material_.showUI(id);
}

std::pair<std::vector<Vertex>, std::vector<unsigned int>> Boid::loadModelFromFile(std::string_view path, bool normalize) {
    const auto basePath = std::filesystem::path(path).parent_path().string() + "/";
    
    tinyobj::ObjReader reader;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    if (!reader.ParseFromFile(path.data())) {
        if (!reader.Error().empty()) {
            throw abcg::RuntimeError(
                fmt::format("Failed to load model {} ({})", path, reader.Error()));
        }
        throw abcg::RuntimeError(fmt::format("Failed to load model {}", path));
    }

    if (!reader.Warning().empty()) {
        fmt::print("Warning: {}\n", reader.Warning());
    }

    std::unordered_map<Vertex, unsigned int> hash;
    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();
    const auto& materials = reader.GetMaterials();

    //lpop over shapes
    for (auto const& shape : shapes) {
        //loop over indices
        for (auto const offset : iter::range(shape.mesh.indices.size())) {
            //access to vertex
            auto const index = shape.mesh.indices.at(offset);

            //vertex position
            const auto startIndex = 3 * index.vertex_index;
            const auto vx = attrib.vertices.at(startIndex + 0);
            const auto vy = attrib.vertices.at(startIndex + 1);
            const auto vz = attrib.vertices.at(startIndex + 2);
            glm::vec3 position = {vx, vy, vz};

            //normal
            glm::vec3 normal;
            if (index.normal_index >= 0) {
                const auto normalStartIndex = 3 * index.normal_index;
                const auto nx = attrib.normals.at(normalStartIndex + 0);
                const auto ny = attrib.normals.at(normalStartIndex + 1);
                const auto nz = attrib.normals.at(normalStartIndex + 2);
                normal = {nx, ny, nz};
            }

            //texture coordinates
            glm::vec2 texCoord;
            if (index.texcoord_index >= 0) {
                const auto texCoordsStartIndex = 2 * index.texcoord_index;
                const auto tx = attrib.texcoords.at(texCoordsStartIndex + 0);
                const auto ty = attrib.texcoords.at(texCoordsStartIndex + 1);
                texCoord = {tx, ty};
            }

            const Vertex vertex {.position = position, .normal = normal, .texCoord = texCoord};

            //if map doesn't contain this vertex
            if (!hash.contains(vertex)) {
                //add this index (size of m_vertices)
                hash[vertex] = vertices.size();
                //add this vertex
                vertices.push_back(vertex);
            }
            indices.push_back(hash[vertex]);
        }
    }

    //use properties of first material, if available
    if (!materials.empty()) {
        const auto& mat = materials.at(0); // First material
        s_Material_.ambient = {mat.ambient[0], mat.ambient[1], mat.ambient[2]};
        s_Material_.diffuse = {mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]};
        s_Material_.specular = {mat.specular[0], mat.specular[1], mat.specular[2]};
        s_Material_.shininess = mat.shininess;

        if (!mat.diffuse_texname.empty()) {
            s_Material_.diffuseTexture = abcg::loadOpenGLTexture({.path = basePath + mat.diffuse_texname});
        }
    }

    if (normalize)
        standardize(vertices);

    return {vertices, indices};
}

void Boid::standardize(std::vector<Vertex>& vertices) {
    //center to origin and normalize bounds to [-1, 1]
    //get bounds
    glm::vec3 max(std::numeric_limits<float>::lowest());
    glm::vec3 min(std::numeric_limits<float>::max());
    for (const auto& vertex : vertices) {
        max = glm::max(max, vertex.position);
        min = glm::min(min, vertex.position);
    }

    //center and scale
    auto const center = (min + max) / 2.0f;
    auto const scaling = 2.0f / glm::length(max - min);
    for (auto& vertex : vertices) {
        vertex.position = (vertex.position - center) * scaling;
    }
}
