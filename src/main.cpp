#include "PCH.h"

#include "Data.h"
#include "Offsets.h"
#include "Plugin.h"
#include "Serialize.h"

void InitializeLogging() {
  auto path = logger::log_directory();
  if (!path) {
    SKSE::stl::report_and_fail("Failed to find standard logging directory"sv);
  }

  auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(path->string(), true);

  auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
  log->set_level(spdlog::level::debug);
  log->flush_on(spdlog::level::debug);

  spdlog::set_default_logger(std::move(log));
  spdlog::set_pattern("[%H:%M:%S.%e] [%l] %v"s);
}

void MessageHandler(SKSE::MessagingInterface::Message *a_msg) {
  switch (a_msg->type) {
  case SKSE::MessagingInterface::kDataLoaded:
    logger::info("Data loaded message received");
    break;
  case SKSE::MessagingInterface::kPostLoad:
    logger::info("Post load message received");
    break;
  case SKSE::MessagingInterface::kNewGame:
  case SKSE::MessagingInterface::kPostLoadGame:
    logger::info("Game changed message received");
    break;
  }
}

SKSEPluginLoad(const SKSE::LoadInterface *a_skse) {
  SKSE::Init(a_skse);

  InitializeLogging();
  logger::info("PapyrusUtil loading...");

  // Allocate trampoline space
  SKSE::AllocTrampoline(1 << 10);

  // Initialize offsets with address library
  if (!Plugin::InitializeOffsets()) {
    logger::error("InitializeOffsets failed!");
    return false;
  }

  // Initialize plugin and data storage
  Plugin::InitPlugin();
  Data::InitLists();

  // Get serialization interface
  auto serialization = SKSE::GetSerializationInterface();
  if (!serialization) {
    logger::error("Couldn't get serialization interface");
    return false;
  }

  // Set up serialization callbacks
  // Using a unique ID for save data (same as before: 884715692 + 227106806 =
  // 1111822498)
  serialization->SetUniqueID('PPUT'); // PapyrusUtil
  serialization->SetSaveCallback(Data::Serialization_Save);
  serialization->SetLoadCallback(Data::Serialization_Load);
  serialization->SetRevertCallback(Data::Serialization_Revert);

  // Get papyrus interface and register functions
  auto papyrus = SKSE::GetPapyrusInterface();
  if (!papyrus) {
    logger::error("Couldn't get papyrus interface");
    return false;
  }

  papyrus->Register(Plugin::RegisterPapyrusFunctions);

  // Register for SKSE messages
  auto messaging = SKSE::GetMessagingInterface();
  if (messaging) {
    messaging->RegisterListener(MessageHandler);
  }

  logger::info("PapyrusUtil loaded successfully");
  return true;
}
