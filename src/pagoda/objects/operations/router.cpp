#include "router.h"

#include <pagoda/objects/procedural_object_predicate.h>
#include <pagoda/objects/procedural_object_predicate_registry.h>
#include <pagoda/objects/procedural_object_system.h>

#include <pagoda/common/debug/assertions.h>
#include <pagoda/common/debug/logger.h>
#include <pagoda/common/instrument/profiler.h>

#include <pagoda/dynamic/string_value.h>

#include <iostream>
#include <memory>

namespace pagoda::objects::operations
{
Router::Router(ProceduralObjectSystemPtr objectSystem) : ProceduralOperation(objectSystem) {}

Router::~Router() {}

void Router::DoWork()
{
	START_PROFILE;

	auto predicateRegistry = m_proceduralObjectSystem->GetPredicateRegistry();

	while (HasInput("in")) {
		auto object = GetInputProceduralObject("in");
		for (const auto &p : *m_memberTable) {
			if (p.second.m_value->GetTypeInfo() == dynamic::String::s_typeInfo) {
				std::string predicateName = p.second.m_value->ToString();
				auto predicate = predicateRegistry->Get(predicateName);

				if (predicate == nullptr) {
					LOG_ERROR("Predicate with name " << p.second.m_value->ToString() << " not found");
					return;
				}

				if ((*predicate)(object)) {
					CreateOutputProceduralObject(object, p.first);
				}
			}
		}
	}
}
}  // namespace pagoda::objects::operations