#include "Serialize.h"

#include <sstream>

// CommonLibSSE-NG types provided via PCH.h

#include "Data.h"
#include "External.h"
#include "Forms.h"
#include "PackageData.h"

// #include <fstream>
// #include <boost/archive/text_oarchive.hpp>
// #include <boost/archive/text_iarchive.hpp>

namespace Data {

const UInt32 kSerializationDataVersion = 1;

// Flat obj=>key=>value storage
extern intv *intValues;
extern flov *floatValues;
extern strv *stringValues;
extern forv *formValues;

// Vector obj=>key=>vector[i] storage
extern intl *intLists;
extern flol *floatLists;
extern strl *stringLists;
extern forl *formLists;

// Overrides
extern forl *packageLists;
// extern aniv* animValues;

template <class T> void Load(T *Data, SKSE::SerializationInterface *intfc, UInt32 &version, UInt32 &length) {
  if (!Data || version != kSerializationDataVersion || length <= 5) {
    return;
  }

  auto buf = std::make_unique<char[]>(length + 1);
  if (!intfc->ReadRecordData(buf.get(), length)) {
    logger::error("\t- failed to read record data");
    return;
  }
  buf[length] = 0;
  std::stringstream ss(buf.get());
  Data->LoadStream(ss);
  logger::info("\t- objects: {}", Data->size());
}

template <class T> void Save(T *Data, SKSE::SerializationInterface *intfc, UInt32 type) {
  if (!Data || Data->empty())
    logger::info("\t- empty!");
  else {
    logger::info("\t- objects: {}", Data->size());
    std::stringstream ss;
    Data->SaveStream(ss);
    if (intfc->OpenRecord(type, kSerializationDataVersion)) {
      const std::string &str = ss.str();
      const char *cstr = str.c_str();
      intfc->WriteRecordData(cstr, static_cast<std::uint32_t>(strlen(cstr)));
    }
  }
}

void FormDelete(UInt64 handle) {
  // All storage pointers are initialized together by InitLists(),
  // but check each before use for safety
  logger::info("Form Delete Handle: {}", handle);
  if (intValues)
    intValues->RemoveForm(handle);
  if (floatValues)
    floatValues->RemoveForm(handle);
  if (stringValues)
    stringValues->RemoveForm(handle);
  if (formValues)
    formValues->RemoveForm(handle);

  if (intLists)
    intLists->RemoveForm(handle);
  if (floatLists)
    floatLists->RemoveForm(handle);
  if (stringLists)
    stringLists->RemoveForm(handle);
  if (formLists)
    formLists->RemoveForm(handle);
}

void Serialization_Load(SKSE::SerializationInterface *intfc) {
  std::uint32_t type;
  std::uint32_t version;
  std::uint32_t length;

  InitLists();

  // Forms::LoadCurrentMods();
  // Forms::LoadModList(intfc);

  logger::info("Storage Loading...");

  while (intfc->GetNextRecordInfo(type, version, length)) {
    switch (type) {
    case 'MODS':
      logger::info("\tMODS Load (old)");
      Forms::LoadModList(intfc);
      break;

    case 'PLGN':
      logger::info("\tPLGN Load");
      Forms::LoadPluginList(intfc);
      break;

    case 'INTV':
      logger::info("\tINTV Load");
      Load(intValues, intfc, version, length);
      break;

    case 'FLOV':
      logger::info("\tFLOV Load");
      Load(floatValues, intfc, version, length);
      break;

    case 'STRV':
      logger::info("\tSTRV Load");
      Load(stringValues, intfc, version, length);
      break;

    case 'FORV':
      logger::info("\tFORV Load");
      Load(formValues, intfc, version, length);
      break;

    case 'INTL':
      logger::info("\tINTL Load");
      Load(intLists, intfc, version, length);
      break;

    case 'FLOL':
      logger::info("\tFLOL Load");
      Load(floatLists, intfc, version, length);
      break;

    case 'STRL':
      logger::info("\tSTRL Load");
      Load(stringLists, intfc, version, length);
      break;

    case 'FORL':
      logger::info("\tFORL Load");
      Load(formLists, intfc, version, length);
      break;

    case 'PKGO':
      logger::info("\tPKGO Load");
      Load(PackageData::GetPackages(), intfc, version, length);
      break;

      /*case 'PACK':
              _MESSAGE("PACK Load");
              Load(packageLists, intfc, version, length);
              break;*/

      /*case 'DATA':
              if (version == kSerializationDataVersion && length > 0) {
                      char *buf = new char[length + 1];
                      intfc->ReadRecordData(buf, length);
                      buf[length] = 0;
                      std::stringstream ss(buf);
                      const std::string &tmp = ss.str();
                      _MESSAGE("DATA: %d", strlen(tmp.c_str()));
                      int ver;
                      ss >> ver;
                      //Forms::LoadPreviousMods(ss);
                      if (ver == -1) {
                              _MESSAGE("-- Legacy Data");
                              intValues->LoadStream(ss);
                              floatValues->LoadStream(ss);
                              stringValues->LoadStream(ss);
                              formValues->LoadStream(ss);

                              intLists->LoadStream(ss);
                              floatLists->LoadStream(ss);
                              stringLists->LoadStream(ss);
                              formLists->LoadStream(ss);
                      }
                      delete[] buf;
              }
              break;*/

    default:
      logger::info("unhandled type {:08X}", type);
      break;
    }
  }
  // Forms::ClearPreviousMods();
  logger::info("Done!\n");
}

void Serialization_Save(SKSE::SerializationInterface *intfc) {
  logger::info("Storage Saving...");

  // Forms::LoadCurrentMods();
  // Forms::ClearPreviousMods();

  // Init lists if for some weird reason unset
  InitLists();

  /*if (intfc->OpenRecord('DATA', kSerializationDataVersion)) {
          std::stringstream ss;
          ss << (int)1;
          Forms::SaveCurrentMods(ss);
          std::string str = ss.str();
          const char *cstr = str.c_str();
          intfc->WriteRecordData(cstr, strlen(cstr));
  }*/
  // Save load order
  // Forms::SaveModList(intfc);

  Forms::SavePluginsList(intfc);
  logger::info("\tPLGN Saved");

  // Cleanup removed forms
  /*int cleaned = 0;
  cleaned += intValues->Cleanup();
  cleaned += floatValues->Cleanup();
  cleaned += stringValues->Cleanup();
  cleaned += formValues->Cleanup();
  cleaned += intLists->Cleanup();
  cleaned += floatLists->Cleanup();
  cleaned += stringLists->Cleanup();
  cleaned += formLists->Cleanup();
  if (cleaned > 0)
          _MESSAGE("- discarded: %d", cleaned);*/

  // Save value storage
  Save(intValues, intfc, 'INTV');
  logger::info("\tINTV Saved");
  Save(floatValues, intfc, 'FLOV');
  logger::info("\tFLOV Saved");
  Save(stringValues, intfc, 'STRV');
  logger::info("\tSTRV Saved");
  Save(formValues, intfc, 'FORV');
  logger::info("\tFORV Saved");

  // Save list storage
  Save(intLists, intfc, 'INTL');
  logger::info("\tINTL Saved");
  Save(floatLists, intfc, 'FLOL');
  logger::info("\tFLOL Saved");
  Save(stringLists, intfc, 'STRL');
  logger::info("\tSTRL Saved");
  Save(formLists, intfc, 'FORL');
  logger::info("\tFORL Saved");

  // Overrides
  Save(PackageData::GetPackages(), intfc, 'PKGO');
  logger::info("\tPKGO Saved");

  // Save external files
  External::SaveFiles();
  logger::info("\tExternal JSON Saved");

  logger::info("Save Done!\n");
}

void Serialization_Revert(SKSE::SerializationInterface *intfc) {
  logger::info("Storage Reverting...");

  logger::info("\t - Mod List");
  Forms::ClearModList();
  // Forms::LoadCurrentMods();
  // Forms::ClearPreviousMods();

  logger::info("\t - StorageUtil: Values");
  if (intValues)
    intValues->Revert();
  if (floatValues)
    floatValues->Revert();
  if (stringValues)
    stringValues->Revert();
  if (formValues)
    formValues->Revert();

  logger::info("\t - StorageUtil: Lists");
  if (intLists)
    intLists->Revert();
  if (floatLists)
    floatLists->Revert();
  if (stringLists)
    stringLists->Revert();
  if (formLists)
    formLists->Revert();

  logger::info("\t - Package Overrides");
  if (auto *packages = PackageData::GetPackages()) {
    packages->Revert();
  }

  // Revert external files
  logger::info("\t - JSON Files");
  External::RevertFiles();

  logger::info("\t - Re-Init");
  InitLists();

  logger::info("Done!\n");

  //_MESSAGE("Revert skip!\n");
}
} // namespace Data
