#include "core/HerodotusEngine.hpp"

class HerodotusEngine;

namespace BoardEvaluation {
// Evaluations the current board position.
double evaluationCurrentPosition(HerodotusEngine &engine);
int countDoubledPawns(HerodotusEngine &engine, Color color);
int countIsolatedPawns(HerodotusEngine &engine, Color color);
int countBlockedPawns(HerodotusEngine &engine, Color color);
} // namespace BoardEvaluation
