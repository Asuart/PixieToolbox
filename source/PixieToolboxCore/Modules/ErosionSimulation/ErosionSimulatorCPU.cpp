#include "ErosionSimulatorCPU.h"

#include <cmath>
#include <random>
#include <cassert>

ErosionSimulatorCPU::ErosionSimulatorCPU(int32_t resolutionX, int32_t resolutionY, float realSizeX, float realSizeY) {
    Resize(resolutionX, resolutionY, realSizeX, realSizeY);
}

void ErosionSimulatorCPU::Run(int32_t steps) {
    for (int32_t i = 0; i < steps; i++) {
        Step();
    }
}

void ErosionSimulatorCPU::Step() {
    IncrementWater();
    UpdateOutflowFlux();
    UpdateWaterSurfaceAndVelocity();
    UpdateErosionAndDeposition();
    SedimentTransport();
    Evaporation();

    m_iteration++;
}

void ErosionSimulatorCPU::Resize(int32_t resolutionX, int32_t resolutionY, float realSizeX, float realSizeY) {
    m_resolutionX = resolutionX;
    m_resolutionY = resolutionY;
    m_realSizeX = realSizeX;
    m_realSizeY = realSizeY;
    m_cellsCount = m_resolutionX * m_resolutionY;
    m_cellSizeX = m_realSizeX / static_cast<float>(m_resolutionX);
    m_cellSizeY = m_realSizeY / static_cast<float>(m_resolutionY);
    m_cellArea = m_cellSizeX * m_cellSizeY;
    m_iteration = 0;

    m_terrainHeight.resize(m_cellsCount);
    m_waterHeight.resize(m_cellsCount);
    m_suspendedSediment.resize(m_cellsCount);
    m_suspendedSedimentUpdated.resize(m_cellsCount);
    m_outflowFluxLeft.resize(m_cellsCount);
    m_outflowFluxRight.resize(m_cellsCount);
    m_outflowFluxTop.resize(m_cellsCount);
    m_outflowFluxBottom.resize(m_cellsCount);
    m_velocityX.resize(m_cellsCount);
    m_velocityY.resize(m_cellsCount);

    m_pipeLength = m_cellSizeX;
    m_inverseCellArea = 1.0f / m_cellArea;
    m_dtGravity_pipeLength = m_dt * m_gravity / m_pipeLength;
    m_dtEvaporationConstant = m_dt * m_evaporationConstant;
}

void ErosionSimulatorCPU::SetInitialHeightMap(const float* const heightMapData, int32_t resolutionX, int32_t resolutionY, float realSizeX, float realSizeY) {
    Resize(resolutionX, resolutionY, realSizeX, realSizeY);
    for (int64_t cellIndex = 0; cellIndex < m_cellsCount; cellIndex++) {
        m_terrainHeight[cellIndex] = heightMapData[cellIndex];
    }
    m_iteration = 0;
}

void ErosionSimulatorCPU::InitializeUniformHeightMap(float height) {
    for (int64_t i = 0; i < m_cellsCount; i++) {
        m_terrainHeight[i] = height;
        m_waterHeight[i] = 0.0f;
        m_suspendedSediment[i] = 0.0f;
        m_suspendedSedimentUpdated[i] = 0.0f;
        m_outflowFluxLeft[i] = 0.0f;
        m_outflowFluxRight[i] = 0.0f;
        m_outflowFluxTop[i] = 0.0f;
        m_outflowFluxBottom[i] = 0.0f;
        m_velocityX[i] = 0.0f;
        m_velocityY[i] = 0.0f;
    }
    m_iteration = 0;
}

void ErosionSimulatorCPU::RandomizeInitialHeighMap(float minHeight, float maxHeight) {
    for (int64_t i = 0; i < m_cellsCount; i++) {
        m_terrainHeight[i] = minHeight + (maxHeight - minHeight) * static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        m_waterHeight[i] = 0.0f;
        m_suspendedSediment[i] = 0.0f;
        m_suspendedSedimentUpdated[i] = 0.0f;
        m_outflowFluxLeft[i] = 0.0f;
        m_outflowFluxRight[i] = 0.0f;
        m_outflowFluxTop[i] = 0.0f;
        m_outflowFluxBottom[i] = 0.0f;
        m_velocityX[i] = 0.0f;
        m_velocityY[i] = 0.0f;
    }
    m_iteration = 0;
}

void ErosionSimulatorCPU::IncrementWater() {
    for (int64_t cellIndex = 0; cellIndex < m_cellsCount; cellIndex++) {
        float waterIncrement = m_waterIncrement * m_dt;
        m_waterHeight[cellIndex] += waterIncrement;

        assert(IsReasonable(m_waterHeight[cellIndex]));
    }
}

void ErosionSimulatorCPU::UpdateOutflowFlux() {
    for (int64_t cellIndex = 0; cellIndex < m_cellsCount; cellIndex++) {
        float outflowLeft = m_outflowFluxLeft[cellIndex];
        float outflowRight = m_outflowFluxRight[cellIndex];
        float outflowTop = m_outflowFluxTop[cellIndex];
        float outflowBottom = m_outflowFluxBottom[cellIndex];

        float terrainHeight = m_terrainHeight[cellIndex];
        float waterHeight = m_waterHeight[cellIndex];
        float cellHeight = terrainHeight + waterHeight;

        if ((cellIndex % m_resolutionX) == 0) {
            outflowLeft = 0;
        }
        else {
            float cellheightLeft = m_terrainHeight[cellIndex - 1] + m_waterHeight[cellIndex - 1];
            float deltaHeight = cellHeight - cellheightLeft;
            float crossSection = waterHeight * m_cellSizeY;
            outflowLeft = fmax(0.0f, outflowLeft + deltaHeight * crossSection * m_dtGravity_pipeLength);
        }

        if (((cellIndex + 1) % m_resolutionX) == 0) {
            outflowRight = 0;
        }
        else {
            float cellheightRight = m_terrainHeight[cellIndex + 1] + m_waterHeight[cellIndex + 1];
            float deltaHeight = cellHeight - cellheightRight;
            float crossSection = waterHeight * m_cellSizeY;
            outflowRight = fmax(0.0f, outflowRight + deltaHeight * crossSection * m_dtGravity_pipeLength);
        }

        if ((cellIndex / m_resolutionX) == 0) {
            outflowTop = 0;
        }
        else {
            float cellheightTop = m_terrainHeight[cellIndex - m_resolutionX] + m_waterHeight[cellIndex - m_resolutionX];
            float deltaHeight = cellHeight - cellheightTop;
            float crossSection = waterHeight * m_cellSizeX;
            outflowTop = fmax(0.0f, outflowTop + deltaHeight * crossSection * m_dtGravity_pipeLength);
        }

        if (((cellIndex / m_resolutionX) + 1) == m_resolutionY) {
            outflowBottom = 0;
        }
        else {
            float cellheightBottom = m_terrainHeight[cellIndex + m_resolutionX] + m_waterHeight[cellIndex + m_resolutionX];
            float deltaHeight = cellHeight - cellheightBottom;
            float crossSection = waterHeight * m_cellSizeX;
            outflowBottom = fmax(0.0f, outflowBottom + deltaHeight * crossSection * m_dtGravity_pipeLength);
        }

        float totalOutflow = outflowLeft + outflowRight + outflowTop + outflowBottom;
        float outflowLimitScale = fmin(1.0f, m_waterHeight[cellIndex] * m_cellArea / (totalOutflow * m_dt));

        assert(IsReasonable(outflowLimitScale));

        m_outflowFluxLeft[cellIndex] = outflowLeft * outflowLimitScale;
        m_outflowFluxRight[cellIndex] = outflowRight * outflowLimitScale;
        m_outflowFluxTop[cellIndex] = outflowTop * outflowLimitScale;
        m_outflowFluxBottom[cellIndex] = outflowBottom * outflowLimitScale;

        assert(IsReasonable(m_outflowFluxLeft[cellIndex]));
        assert(IsReasonable(m_outflowFluxRight[cellIndex]));
        assert(IsReasonable(m_outflowFluxTop[cellIndex]));
        assert(IsReasonable(m_outflowFluxBottom[cellIndex]));
    }
}

void ErosionSimulatorCPU::UpdateWaterSurfaceAndVelocity() {
    for (int64_t cellIndex = 0; cellIndex < m_cellsCount; cellIndex++) {
        float initialWaterHeight = m_waterHeight[cellIndex];

        float outflowLeft = m_outflowFluxLeft[cellIndex];
        float inflowLeft = 0.0f;
        if ((cellIndex % m_resolutionX) != 0) {
            inflowLeft = m_outflowFluxRight[cellIndex - 1];
        }

        float outflowRight = m_outflowFluxRight[cellIndex];
        float inflowRight = 0.0f;
        if (((cellIndex + 1) % m_resolutionX) != 0) {
            inflowRight = m_outflowFluxLeft[cellIndex + 1];
        }

        float outflowTop = m_outflowFluxTop[cellIndex];
        float inflowTop = 0.0f;
        if ((cellIndex / m_resolutionX) != 0) {
            inflowTop = m_outflowFluxBottom[cellIndex - m_resolutionX];
        }

        float outflowBottom = m_outflowFluxBottom[cellIndex];
        float inflowBottom = 0.0f;
        if (((cellIndex / m_resolutionX) + 1) != m_resolutionY) {
            inflowBottom = m_outflowFluxTop[cellIndex + m_resolutionX];
        }

        float totalOutflow = outflowLeft + outflowRight + outflowTop + outflowBottom;
        float totalInflow = inflowLeft + inflowRight + inflowTop + inflowBottom;
        float volumeChange = (totalInflow - totalOutflow) * m_dt;

        float updatedWaterHeight = initialWaterHeight + volumeChange * m_inverseCellArea;
        m_waterHeight[cellIndex] = updatedWaterHeight;

        //assert(updatedWaterHeight >= 0.0f);
        assert(IsReasonable(updatedWaterHeight));

        float averageWaterHeight = (initialWaterHeight + updatedWaterHeight) * 0.5f;
        if (averageWaterHeight < 0.001f) {
            m_velocityX[cellIndex] = 0.0f;
            m_velocityY[cellIndex] = 0.0f;
            continue;
        }

        float waterFlowX = (inflowLeft - outflowLeft + outflowRight - inflowRight) * 0.5f;
        float waterFlowY = (inflowTop - outflowTop + outflowBottom - inflowBottom) * 0.5f;

        float velocityX = waterFlowX / (m_cellSizeY * averageWaterHeight);
        float velocityY = waterFlowY / (m_cellSizeX * averageWaterHeight);

        m_velocityX[cellIndex] = velocityX;
        m_velocityY[cellIndex] = velocityY;

        assert(IsReasonable(velocityX));
        assert(IsReasonable(velocityY));

        assert((velocityX * m_dt) < m_cellSizeX);
        assert((velocityY * m_dt) < m_cellSizeY);

        assert(fabsf(velocityX * m_dt) < 1.0f);
        assert(fabsf(velocityY * m_dt) < 1.0f);
    }
}

void ErosionSimulatorCPU::UpdateErosionAndDeposition() {
    for (int64_t y = 0, cellIndex = 0; y < m_resolutionY; y++) {
        for (int64_t x = 0; x < m_resolutionX; x++, cellIndex++) {
            float tilt = SampleTilt(x, y);
            float tiltScale = fmax(m_minTiltScale, sinf(tilt));
            float speed = SampleSpeed(cellIndex);
            float capacity = m_sedimentCapacityConstant * tiltScale * speed;
            float suspendedSediment = m_suspendedSediment[cellIndex];

            if (capacity > suspendedSediment) {
                float dissolvedAmount = m_sedimentDissolveConstant * (capacity - suspendedSediment);
                m_terrainHeight[cellIndex] -= dissolvedAmount;
                m_suspendedSediment[cellIndex] += dissolvedAmount;

                //assert(m_terrainHeight[cellIndex] >= 0.0f);
                assert(IsReasonable(m_terrainHeight[cellIndex]));
                assert(IsReasonable(m_suspendedSediment[cellIndex]));
            }
            else {
                float depositedAmount = m_sedimentDepositionConstant * (suspendedSediment - capacity);
                m_terrainHeight[cellIndex] += depositedAmount;
                m_suspendedSediment[cellIndex] -= depositedAmount;

                assert(IsReasonable(m_suspendedSediment[cellIndex] >= 0.0f));
                assert(IsReasonable(m_terrainHeight[cellIndex]));
                assert(IsReasonable(m_suspendedSediment[cellIndex]));
            }
        }
    }
}

void ErosionSimulatorCPU::SedimentTransport() {
    for (int64_t y = 0, cellIndex = 0; y < m_resolutionY; y++) {
        for (int64_t x = 0; x < m_resolutionX; x++, cellIndex++) {
            float offsetX = m_velocityX[cellIndex] * m_dt;
            float offsetY = m_velocityY[cellIndex] * m_dt;
            float absOffsetX = fmin(1.0f, fabsf(offsetX));
            float absOffsetY = fmin(1.0f, fabsf(offsetY));
            int32_t intOffsetX = offsetX >= 0 ? 1 : -1;
            int32_t intOffsetY = offsetY >= 0 ? 1 : -1;

            float v00 = m_suspendedSediment[cellIndex];
            float v10 = m_suspendedSediment[CoordsToIndexClamped(x + intOffsetX, y)];
            float v01 = m_suspendedSediment[CoordsToIndexClamped(x, y + intOffsetY)];
            float v11 = m_suspendedSediment[CoordsToIndexClamped(x + intOffsetX, y + intOffsetY)];

            float bilinearlyInterpolated = 
                v00 * (1.0f - absOffsetX) * (1.0f - absOffsetY) +
                v10 * absOffsetX * (1.0f - absOffsetY) +
                v01 * (1.0f - absOffsetX) * absOffsetY +
                v11 * absOffsetX * absOffsetY;

            m_suspendedSedimentUpdated[cellIndex] = bilinearlyInterpolated;

            assert(IsReasonable(bilinearlyInterpolated));
        }
    }
    std::swap(m_suspendedSediment, m_suspendedSedimentUpdated);
}

void ErosionSimulatorCPU::Evaporation() {
    for (int64_t cellIndex = 0; cellIndex < m_cellsCount; cellIndex++) {
        m_waterHeight[cellIndex] -= m_waterHeight[cellIndex] * m_dtEvaporationConstant;
    }
}

float ErosionSimulatorCPU::SampleTilt(int64_t x, int64_t y) {
    float heightLeft = m_terrainHeight[CoordsToIndexClamped(x - 1, y)];
    float heightRight = m_terrainHeight[CoordsToIndexClamped(x + 1, y)];
    float heightTop = m_terrainHeight[CoordsToIndexClamped(x, y - 1)];
    float heightBottom = m_terrainHeight[CoordsToIndexClamped(x, y + 1)];
    float dx = heightRight - heightLeft;
    float dy = heightBottom - heightTop;
    return atanf(sqrtf(dx * dx + dy * dy));
}

float ErosionSimulatorCPU::SampleSpeed(int64_t cellIndex) {
    return sqrtf(m_velocityX[cellIndex] * m_velocityX[cellIndex] + m_velocityY[cellIndex] * m_velocityY[cellIndex]);
}

int64_t ErosionSimulatorCPU::CoordsToIndexClamped(int64_t x, int64_t y) {
    if (x < 0) x = 0;
    else if (x >= m_resolutionX) x = m_resolutionX - 1;
    if (y < 0) y = 0;
    else if (y >= m_resolutionY) y = m_resolutionY - 1;
    return y * m_resolutionX + x;
}

bool ErosionSimulatorCPU::IsReasonable(float value) {
    return isfinite(value) && (fabsf(value) < 1000.0f);
}
