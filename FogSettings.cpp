#include "FogSettings.h"

void FogSettings::Apply(Shader& shader) const
{
    shader.SetBool("fog.enabled", enabled);

    shader.SetVec3("fog.localLayer.color", localLayer.color);
    shader.SetFloat("fog.localLayer.nearDistance", localLayer.nearDistance);
    shader.SetFloat("fog.localLayer.farDistance", localLayer.farDistance);
    shader.SetFloat("fog.localLayer.density", localLayer.density);
    shader.SetFloat("fog.localLayer.baseHeight", localLayer.baseHeight);
    shader.SetFloat("fog.localLayer.heightFalloff", localLayer.heightFalloff);
    shader.SetFloat("fog.localLayer.maxOpacity", localLayer.maxOpacity);
    shader.SetFloat("fog.localLayer.solidCutoff", localLayer.solidCutoff);

    shader.SetVec3("fog.boundaryLayer.color", boundaryLayer.color);
    shader.SetFloat("fog.boundaryLayer.nearDistance", boundaryLayer.nearDistance);
    shader.SetFloat("fog.boundaryLayer.farDistance", boundaryLayer.farDistance);
    shader.SetFloat("fog.boundaryLayer.density", boundaryLayer.density);
    shader.SetFloat("fog.boundaryLayer.baseHeight", boundaryLayer.baseHeight);
    shader.SetFloat("fog.boundaryLayer.heightFalloff", boundaryLayer.heightFalloff);
    shader.SetFloat("fog.boundaryLayer.maxOpacity", boundaryLayer.maxOpacity);
    shader.SetFloat("fog.boundaryLayer.solidCutoff", boundaryLayer.solidCutoff);
}
