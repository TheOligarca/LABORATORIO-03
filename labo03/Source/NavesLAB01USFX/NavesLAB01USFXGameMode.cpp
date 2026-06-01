// Copyright Epic Games, Inc. All Rights Reserved.

#include "NavesLAB01USFXGameMode.h"
#include "NavesLAB01USFXPawn.h"
#include "Enemigo.h"
#include "EnemigoAereo.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/World.h"

ANavesLAB01USFXGameMode::ANavesLAB01USFXGameMode()
{
	DefaultPawnClass = ANavesLAB01USFXPawn::StaticClass();
	PrimaryActorTick.bCanEverTick = true;
}

// ---------------------------------------------------------------------------
// BeginPlay: spawn de naves + inicio del ciclo de timers
// ---------------------------------------------------------------------------
void ANavesLAB01USFXGameMode::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("=== NavesLAB01USFX - BeginPlay ==="));

	// Obtener referencia al Pawn del jugador
	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (!PlayerPawn)
	{
		UE_LOG(LogTemp, Error, TEXT("No se encontró el PlayerPawn. Asegúrate de que existe en el nivel."));
	}

	// Spawnear las 20 naves con movimiento autónomo activado
	SpawnearNaves();

	// ---- Ciclo de timers ----
	// Después de TiempoMovimientoAutonomo segundos → iniciar formación
	// Después de TiempoEnFormacion segundos más    → reanudar movimiento autónomo
	// El timer de formación se repite con período (TiempoMovimientoAutonomo + TiempoEnFormacion)
	float PeriodoCiclo = TiempoMovimientoAutonomo + TiempoEnFormacion;

	GetWorldTimerManager().SetTimer(
		TimerIniciarFormacion,
		this,
		&ANavesLAB01USFXGameMode::IniciarFormacion,
		PeriodoCiclo,   // Cada ciclo completo
		true,           // Se repite
		TiempoMovimientoAutonomo  // Primera vez: después de 5s
	);

	UE_LOG(LogTemp, Warning, TEXT("Ciclo iniciado: %.1fs movimiento → %.1fs formación → repetir"),
		TiempoMovimientoAutonomo, TiempoEnFormacion);
}

void ANavesLAB01USFXGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// Lógica adicional de juego puede ir aquí
}

// ---------------------------------------------------------------------------
// SpawnearNaves: crea CantidadNaves naves AEnemigoAereo dispersas en el escenario
// ---------------------------------------------------------------------------
void ANavesLAB01USFXGameMode::SpawnearNaves()
{
	UWorld* World = GetWorld();
	if (!World) return;

	ListaNaves.Empty();

	// Distribuir naves en una cuadrícula 5x4 con algo de aleatoriedad
	const int32 Columnas = 5;
	const float SeparacionX = 300.0f;
	const float SeparacionY = 300.0f;
	const float OffsetAleatorio = 80.0f;
	const float AlturaSpawn = 200.0f;

	for (int32 i = 0; i < CantidadNaves; i++)
	{
		int32 Fila    = i / Columnas;
		int32 Columna = i % Columnas;

		float PosX = -600.0f + (Columna * SeparacionX) + FMath::RandRange(-OffsetAleatorio, OffsetAleatorio);
		float PosY = -300.0f + (Fila    * SeparacionY) + FMath::RandRange(-OffsetAleatorio, OffsetAleatorio);
		FVector UbicacionSpawn(PosX, PosY, AlturaSpawn);

		// Spawnear como AEnemigoAereo (propuesta espiral)
		AEnemigoAereo* NaveAerea = World->SpawnActor<AEnemigoAereo>(
			AEnemigoAereo::StaticClass(),
			UbicacionSpawn,
			FRotator::ZeroRotator
		);

		if (NaveAerea)
		{
			// Asignar índice para que la espiral tenga fase única
			NaveAerea->IndiceNave = i;

			// BeginPlay ya fue llamado por SpawnActor; necesitamos reinicializar
			// el ángulo con el índice correcto
			NaveAerea->IndiceNave = i;

			ListaNaves.Add(NaveAerea);
			UE_LOG(LogTemp, Log, TEXT("Nave %d spawnada en %s"), i, *UbicacionSpawn.ToString());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Error al spawnear nave %d"), i);
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("Total naves spawnadas: %d"), ListaNaves.Num());
}

// ---------------------------------------------------------------------------
// IniciarFormacion: calcula posiciones frente al Pawn y las envía a las naves
// ---------------------------------------------------------------------------
void ANavesLAB01USFXGameMode::IniciarFormacion()
{
	if (!PlayerPawn)
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	}

	if (!PlayerPawn || ListaNaves.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("IniciarFormacion: PlayerPawn o ListaNaves no disponibles."));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("=== FORMACIÓN INICIADA (%d naves) ==="), ListaNaves.Num());

	FVector   UbicacionPawn  = PlayerPawn->GetActorLocation();
	FRotator  RotacionPawn   = PlayerPawn->GetActorRotation();
	FVector   FrentePawn     = RotacionPawn.Vector(); // Vector unitario hacia adelante

	// Posición base de la formación: frente al Pawn
	FVector PosicionBase = UbicacionPawn + FrentePawn * DistanciaFrenteAlPawn;

	// Vector lateral perpendicular al frente del Pawn (para la fila)
	FVector Lateral = FVector::CrossProduct(FrentePawn, FVector::UpVector).GetSafeNormal();

	// Calcular offset para centrar la fila
	float OffsetCentro = ((ListaNaves.Num() - 1) * EspaciadoFormacion) * 0.5f;

	for (int32 i = 0; i < ListaNaves.Num(); i++)
	{
		if (AEnemigo* Nave = ListaNaves[i])
		{
			// Distribuir las naves en fila lateral frente al Pawn
			float DesplazamientoLateral = (i * EspaciadoFormacion) - OffsetCentro;
			FVector PosicionFormacion = PosicionBase + Lateral * DesplazamientoLateral;
			// Mantener misma altura que la nave actualmente tiene
			PosicionFormacion.Z = Nave->GetActorLocation().Z;

			// Enviar a la nave su posición de formación
			Nave->IniciarFormacion(PosicionFormacion);
		}
	}

	// Después de TiempoEnFormacion segundos → reanudar movimiento autónomo
	GetWorldTimerManager().SetTimer(
		TimerReanudarMovimiento,
		this,
		&ANavesLAB01USFXGameMode::ReanudarMovimientoAutonomo,
		TiempoEnFormacion,
		false  // Una sola vez
	);
}

// ---------------------------------------------------------------------------
// ReanudarMovimientoAutonomo: ordena a todas las naves que reanuden su patrón
// ---------------------------------------------------------------------------
void ANavesLAB01USFXGameMode::ReanudarMovimientoAutonomo()
{
	UE_LOG(LogTemp, Warning, TEXT("=== REANUDANDO MOVIMIENTO AUTÓNOMO ==="));

	for (AEnemigo* Nave : ListaNaves)
	{
		if (Nave)
		{
			Nave->ReanudarMovimientoAutonomo();
		}
	}
}
