#pragma once

#include "PCH.h"

namespace Data {
	void FormDelete(UInt64 handle);
	void Serialization_Load(SKSE::SerializationInterface* intfc);
	void Serialization_Save(SKSE::SerializationInterface* intfc);
	void Serialization_Revert(SKSE::SerializationInterface* intfc);
}
