// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "NavesLAB01USFXGameMode.generated.h"

class AEnemigo;
class APawn;

/**
 * ANavesLAB01USFXGameMode
 *
 * Ciclo de comportamiento de las 20 naves:
 *
 *  [Inicio del juego]
 *       │
 *       ▼
 *  Spawn 20 naves → Movimiento autónomo (espiral)
 *       │  5 segundos
 *       ▼
 *  Formación frente al Pawn (5 segundos)
 *       │  5 segundos
 *       ▼
 *  Reanudan movimiento autónomo
 *       │  5 segundos
 *       ▼
 *  [repite]
 */
UCLASS(MinimalAPI)
class ANavesLAB01USFXGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ANavesLAB01USFXGameMode();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ---- Contenedor de naves (mínimo 20) ----
	UPROPERTY()
	TArray<AEnemigo*> ListaNaves;

	// ---- Referencia al Pawn jugador ----
	UPROPERTY()
	APawn* PlayerPawn;

	// ---- Timers ----
	FTimerHandle TimerIniciarFormacion;       // Dispara formación cada ciclo
	FTimerHandle TimerReanudarMovimiento;     // Dispara reanudación tras 5s en formación

	// ---- Parámetros configurables ----

	// Total de naves a spawnear
	UPROPERTY(EditAnywhere, Category = "Naves")
	int32 CantidadNaves = 20;

	// Segundos de movimiento autónomo antes de formar
	UPROPERTY(EditAnywhere, Category = "Naves")
	float TiempoMovimientoAutonomo = 5.0f;

	// Segundos en formación frente al Pawn
	UPROPERTY(EditAnywhere, Category = "Naves")
	float TiempoEnFormacion = 5.0f;

	// Separación lateral entre naves en formación
	UPROPERTY(EditAnywhere, Category = "Naves")
	float EspaciadoFormacion = 120.0f;

	// Distancia frente al Pawn donde se forma la fila
	UPROPERTY(EditAnywhere, Category = "Naves")
	float DistanciaFrenteAlPawn = 400.0f;

private:
	// ---- Métodos internos ----

	// Spawnea las 20 naves y activa movimiento autónomo
	void SpawnearNaves();

	// Calcula posiciones de formación y las envía a cada nave
	void IniciarFormacion();

	// Ordena a todas las naves que reanuden movimiento autónomo
	void ReanudarMovimientoAutonomo();
};
