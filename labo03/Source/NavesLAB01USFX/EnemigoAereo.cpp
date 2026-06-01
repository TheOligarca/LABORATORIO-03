// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemigoAereo.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AEnemigoAereo::AEnemigoAereo()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemigoAereo::BeginPlay()
{
	Super::BeginPlay();

	// Ángulo inicial basado en el índice de la nave:
	// Distribuye las 20 naves uniformemente en el círculo (360° / 20 = 18°)
	AnguloInicial = IndiceNave * (2.0f * PI / 20.0f);
	AnguloActual  = AnguloInicial;

	// Radio inicial: cada nave empieza en una posición distinta de la espiral
	RadioActual = RadioMin + (IndiceNave * (RadioMax - RadioMin) / 20.0f);

	// Alternar expansión/contracción por paridad del índice
	bExpandiendo = (IndiceNave % 2 == 0);

	EstadoActual = EEstadoNave::MovimientoAutonomo;
	bMovimientoAutonomo = true;

	UE_LOG(LogTemp, Log, TEXT("[EnemigoAereo %d] BeginPlay - Angulo: %.2f, Radio: %.2f"),
		IndiceNave, FMath::RadiansToDegrees(AnguloInicial), RadioActual);
}

void AEnemigoAereo::Tick(float DeltaTime)
{
	// Delegamos al Tick del padre, que ya maneja los estados
	Super::Tick(DeltaTime);
}

// ---------------------------------------------------------------------------
// PROPUESTA DE MOVIMIENTO AUTÓNOMO: ESPIRAL CIRCULAR EXPANSIVA/CONTRACTIVA
//
// Descripción para Maria Belén Coa - USFX LAB01:
//
// Cada nave AEnemigoAereo se mueve describiendo una espiral circular alrededor
// de su posición inicial. El radio de la espiral aumenta progresivamente
// (fase expansiva) hasta alcanzar un radio máximo, momento en el cual
// la espiral invierte y se contrae hacia el centro (fase contractiva).
// Este ciclo se repite indefinidamente, produciendo un movimiento orgánico
// similar al vuelo de aves o insectos en enjambre.
//
// Para garantizar independencia visual entre las 20 naves:
//   - Cada nave tiene un ángulo inicial único: IndiceNave * (360° / 20)
//   - El radio inicial también varía por nave
//   - Naves pares expanden primero; naves impares contraen primero
//
// Parámetros clave:
//   VelocidadAngular   = 1.5 rad/s  (velocidad de giro)
//   VelocidadRadio     = 80 uu/s    (velocidad de expansión/contracción)
//   RadioMin           = 50 uu
//   RadioMax           = 500 uu
//   AlturaVuelo        = 200 uu     (Z constante, adecuado para vista top-down)
// ---------------------------------------------------------------------------
void AEnemigoAereo::MoverAutonomo(float DeltaTime)
{
	// 1. Actualizar ángulo de giro
	AnguloActual += VelocidadAngular * DeltaTime;

	// 2. Actualizar radio (expansión o contracción)
	if (bExpandiendo)
	{
		RadioActual += VelocidadRadio * DeltaTime;
		if (RadioActual >= RadioMax)
		{
			RadioActual  = RadioMax;
			bExpandiendo = false; // Invertir: ahora se contrae
		}
	}
	else
	{
		RadioActual -= VelocidadRadio * DeltaTime;
		if (RadioActual <= RadioMin)
		{
			RadioActual  = RadioMin;
			bExpandiendo = true; // Invertir: ahora se expande
		}
	}

	// 3. Calcular nueva posición en la espiral
	float NuevoX = PosicionInicial.X + RadioActual * FMath::Cos(AnguloActual);
	float NuevoY = PosicionInicial.Y + RadioActual * FMath::Sin(AnguloActual);
	FVector NuevaPosicion(NuevoX, NuevoY, AlturaVuelo);

	// 4. Calcular dirección de movimiento para rotar la nave
	FVector Direccion = (NuevaPosicion - GetActorLocation()).GetSafeNormal();
	if (!Direccion.IsNearlyZero())
	{
		FRotator NuevaRotacion = Direccion.Rotation();
		SetActorRotation(NuevaRotacion);
	}

	// 5. Mover la nave a la nueva posición
	SetActorLocation(NuevaPosicion);
}

// Las naves aéreas no usan waypoints tradicionales; ruta vacía.
void AEnemigoAereo::CargarRuta()
{
	PosicionInicial = GetActorLocation();
	PuntosRuta.Empty();
	IndicePuntoRutaActual = 0;
}
