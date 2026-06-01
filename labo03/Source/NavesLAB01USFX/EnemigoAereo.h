// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemigo.h"
#include "EnemigoAereo.generated.h"

/**
 * AEnemigoAereo
 *
 * Propuesta de movimiento autónomo: ESPIRAL CIRCULAR EXPANSIVA
 * ---------------------------------------------------------------
 * Cada nave aérea se mueve describiendo una espiral que parte desde
 * su posición inicial y se expande progresivamente. Cuando alcanza
 * el radio máximo, la espiral se contrae volviendo al centro.
 * El ángulo de inicio es único para cada nave (basado en índice),
 * lo que hace que todas orbiten en fases distintas, generando un
 * efecto visual de enjambre dinámico.
 *
 * Registrado para: Maria Belén Coa - USFX LAB01
 */
UCLASS()
class NAVESLAB01USFX_API AEnemigoAereo : public AEnemigo
{
	GENERATED_BODY()

public:
	AEnemigoAereo();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Sobreescribe el movimiento autónomo con patrón espiral
	virtual void MoverAutonomo(float DeltaTime) override;

	virtual void CargarRuta() override;

private:
	// ---- Parámetros de la espiral ----

	// Radio actual de la espiral
	float RadioActual = 0.0f;

	// Radio mínimo y máximo de la espiral
	float RadioMin = 50.0f;
	float RadioMax = 500.0f;

	// Velocidad angular (radianes/segundo)
	float VelocidadAngular = 1.5f;

	// Velocidad de expansión/contracción del radio
	float VelocidadRadio = 80.0f;

	// Ángulo inicial único por nave (asignado en BeginPlay)
	float AnguloInicial = 0.0f;

	// Ángulo actual de la espiral
	float AnguloActual = 0.0f;

	// Si la espiral está expandiéndose (true) o contrayéndose (false)
	bool bExpandiendo = true;

	// Altura de vuelo (Z fija para vista top-down)
	float AlturaVuelo = 200.0f;

public:
	// Índice de la nave (asignado por el GameMode para diferencia de fase)
	int32 IndiceNave = 0;
};
