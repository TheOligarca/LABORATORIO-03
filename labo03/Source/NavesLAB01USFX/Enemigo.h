// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemigo.generated.h"

class UStaticMeshComponent;

// Estados posibles de cada nave
UENUM(BlueprintType)
enum class EEstadoNave : uint8
{
    MovimientoAutonomo,   // Moviéndose libremente con patrón propio
    Formando,             // Desplazándose hacia posición de formación
    EnFormacion           // Ya está en posición frente al Pawn
};

UCLASS()
class NAVESLAB01USFX_API AEnemigo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemigo();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MallaEnemigo;

	// Velocidad de movimiento autónomo
	UPROPERTY(EditAnywhere, Category = "Movimiento")
	float VelocidadMovimiento = 200.0f;

	// Velocidad al ir a formación (más rápida para llegar a tiempo)
	UPROPERTY(EditAnywhere, Category = "Movimiento")
	float VelocidadFormacion = 400.0f;

	FVector WorldLimitesMin = FVector(-1000, -1000, 0);
	FVector WorldLimitesMax = FVector(1000, 1000, 0);

public:
	// Ruta de waypoints para movimiento tipo patrulla
	TArray<FVector> PuntosRuta;
	int32 IndicePuntoRutaActual = 0;

	FVector PosicionInicial;
	float Tolerancia = 50.0f;

	// Tiempo acumulado (usado para movimientos basados en tiempo como espiral)
	float TiempoAcumulado = 0.0f;

	// ---- Estado de la nave ----
	EEstadoNave EstadoActual = EEstadoNave::MovimientoAutonomo;

	// Destino cuando va en formación (asignado por el GameMode)
	FVector PosicionDestinoFormacion;

	// ---- Compatibilidad con código anterior ----
	bool bMovimientoAutonomo = true;
	FVector PosicionDestinoGameMode;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	// Ejecuta el movimiento autónomo de la nave (cada subclase puede sobreescribir)
	virtual void MoverAutonomo(float DeltaTime);

	// Ejecuta el movimiento hacia la posición de formación
	void MoverHaciaFormacion(float DeltaTime);

	// Carga la ruta de movimiento autónomo (sobreescribir en subclases)
	virtual void CargarRuta();

	// Activa el modo formación, asignando posición destino
	void IniciarFormacion(const FVector& PosicionDestino);

	// Regresa al movimiento autónomo
	void ReanudarMovimientoAutonomo();

	// Estado actual visible para el GameMode
	EEstadoNave GetEstado() const { return EstadoActual; }
};
