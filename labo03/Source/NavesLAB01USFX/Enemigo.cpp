// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemigo.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

AEnemigo::AEnemigo()
{
	PrimaryActorTick.bCanEverTick = true;

	MallaEnemigo = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MallaEnemigo"));
	RootComponent = MallaEnemigo;

	// Malla por defecto: torus del Starter Content
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MallaAsset(
		TEXT("StaticMesh'/Game/StarterContent/Shapes/Shape_Torus.Shape_Torus'")
	);
	if (MallaAsset.Succeeded())
	{
		MallaEnemigo->SetStaticMesh(MallaAsset.Object);
	}
}

void AEnemigo::BeginPlay()
{
	Super::BeginPlay();
	PosicionInicial = GetActorLocation();
	CargarRuta();
	EstadoActual = EEstadoNave::MovimientoAutonomo;
	bMovimientoAutonomo = true;
}

void AEnemigo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TiempoAcumulado += DeltaTime;

	switch (EstadoActual)
	{
	case EEstadoNave::MovimientoAutonomo:
		MoverAutonomo(DeltaTime);
		break;

	case EEstadoNave::Formando:
		MoverHaciaFormacion(DeltaTime);
		break;

	case EEstadoNave::EnFormacion:
		// Quieta en posición de formación, no hace nada
		break;
	}
}

// ---------------------------------------------------------------------------
// Movimiento autónomo BASE: sigue waypoints aleatorios en línea recta.
// Las subclases (AEnemigoAereo, etc.) sobreescriben este método con
// su propio patrón exclusivo.
// ---------------------------------------------------------------------------
void AEnemigo::MoverAutonomo(float DeltaTime)
{
	if (PuntosRuta.Num() <= 1) return;

	FVector UbicacionActual = GetActorLocation();
	FVector UbicacionDestino = PuntosRuta[IndicePuntoRutaActual];

	float Distancia = FVector::Dist(UbicacionActual, UbicacionDestino);

	if (Distancia <= Tolerancia)
	{
		// Waypoint alcanzado → avanzar al siguiente (cíclico)
		IndicePuntoRutaActual = (IndicePuntoRutaActual + 1) % PuntosRuta.Num();
	}
	else
	{
		FVector Direccion = (UbicacionDestino - UbicacionActual).GetSafeNormal();
		FVector NuevaUbicacion = UbicacionActual + (Direccion * VelocidadMovimiento * DeltaTime);
		SetActorLocation(NuevaUbicacion);

		// Rotar hacia la dirección de movimiento
		if (!Direccion.IsNearlyZero())
		{
			FRotator NuevaRotacion = Direccion.Rotation();
			SetActorRotation(NuevaRotacion);
		}
	}
}

// ---------------------------------------------------------------------------
// Movimiento hacia posición de formación: interpolación suave
// ---------------------------------------------------------------------------
void AEnemigo::MoverHaciaFormacion(float DeltaTime)
{
	FVector PosicionActual = GetActorLocation();
	float Distancia = FVector::Dist(PosicionActual, PosicionDestinoFormacion);

	if (Distancia <= Tolerancia)
	{
		// Llegó a la formación
		SetActorLocation(PosicionDestinoFormacion);
		EstadoActual = EEstadoNave::EnFormacion;
		UE_LOG(LogTemp, Log, TEXT("[%s] Llegó a la formación."), *GetName());
	}
	else
	{
		// Interpolación lineal suave hacia el destino
		FVector NuevaPos = FMath::VInterpTo(PosicionActual, PosicionDestinoFormacion, DeltaTime, 3.0f);
		SetActorLocation(NuevaPos);
	}
}

// ---------------------------------------------------------------------------
// CargarRuta base: 10 waypoints aleatorios dentro de los límites del mundo
// ---------------------------------------------------------------------------
void AEnemigo::CargarRuta()
{
	PosicionInicial = GetActorLocation();
	PuntosRuta.Empty();
	PuntosRuta.Add(PosicionInicial);

	for (int32 i = 0; i < 10; i++)
	{
		float X = FMath::RandRange(WorldLimitesMin.X, WorldLimitesMax.X);
		float Y = FMath::RandRange(WorldLimitesMin.Y, WorldLimitesMax.Y);
		PuntosRuta.Add(FVector(X, Y, PosicionInicial.Z));
	}

	IndicePuntoRutaActual = 1;
}

// ---------------------------------------------------------------------------
// Activa modo formación
// ---------------------------------------------------------------------------
void AEnemigo::IniciarFormacion(const FVector& PosicionDestino)
{
	PosicionDestinoFormacion = PosicionDestino;
	PosicionDestinoGameMode  = PosicionDestino; // compatibilidad
	EstadoActual = EEstadoNave::Formando;
	bMovimientoAutonomo = false;
	UE_LOG(LogTemp, Log, TEXT("[%s] Iniciando formación hacia %s"), *GetName(), *PosicionDestino.ToString());
}

// ---------------------------------------------------------------------------
// Reanuda movimiento autónomo
// ---------------------------------------------------------------------------
void AEnemigo::ReanudarMovimientoAutonomo()
{
	EstadoActual = EEstadoNave::MovimientoAutonomo;
	bMovimientoAutonomo = true;
	// Recargar ruta para que retome desde su posición actual
	CargarRuta();
	UE_LOG(LogTemp, Log, TEXT("[%s] Reanudando movimiento autónomo."), *GetName());
}
