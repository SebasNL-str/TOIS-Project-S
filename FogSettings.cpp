#include "FogSettings.h"

void FogSettings::Apply(Shader& shader) const
{
    // Activar o desactivar el efecto global de niebla || Enable or disable the global fog effect
    shader.SetBool("fog.enabled", enabled);

    // Transferir parametros de configuracion para la capa de niebla local || Transfer configuration parameters for local fog layer
    shader.SetVec3("fog.localLayer.color", localLayer.color);
    shader.SetFloat("fog.localLayer.nearDistance", localLayer.nearDistance);
    shader.SetFloat("fog.localLayer.farDistance", localLayer.farDistance);
    shader.SetFloat("fog.localLayer.density", localLayer.density);
    shader.SetFloat("fog.localLayer.baseHeight", localLayer.baseHeight);
    shader.SetFloat("fog.localLayer.heightFalloff", localLayer.heightFalloff);
    shader.SetFloat("fog.localLayer.maxOpacity", localLayer.maxOpacity);
    shader.SetFloat("fog.localLayer.solidCutoff", localLayer.solidCutoff);

    // Transferir parametros de configuracion para la capa de niebla de limite || Transfer configuration parameters for boundary fog layer
    shader.SetVec3("fog.boundaryLayer.color", boundaryLayer.color);
    shader.SetFloat("fog.boundaryLayer.nearDistance", boundaryLayer.nearDistance);
    shader.SetFloat("fog.boundaryLayer.farDistance", boundaryLayer.farDistance);
    shader.SetFloat("fog.boundaryLayer.density", boundaryLayer.density);
    shader.SetFloat("fog.boundaryLayer.baseHeight", boundaryLayer.baseHeight);
    shader.SetFloat("fog.boundaryLayer.heightFalloff", boundaryLayer.heightFalloff);
    shader.SetFloat("fog.boundaryLayer.maxOpacity", boundaryLayer.maxOpacity);
    shader.SetFloat("fog.boundaryLayer.solidCutoff", boundaryLayer.solidCutoff);
}