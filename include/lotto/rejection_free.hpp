#ifndef REJECTION_FREE_H
#define REJECTION_FREE_H

#include <cassert>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

#include "event_rate_tree.hpp"
#include "event_rate_tree_impl.hpp"
#include "event_selector.hpp"

class RejectionFreeEventSelectorTest;

namespace lotto {

template <typename EventIDType>
struct GetImpactFromTable {
  GetImpactFromTable(
      std::map<EventIDType, std::vector<EventIDType>> const &_impact_table)
      : impact_table(_impact_table) {}

  std::vector<EventIDType> const &operator()(
      const EventIDType &event_id) const {
    return impact_table.at(event_id);
  }

  std::map<EventIDType, std::vector<EventIDType>> const &impact_table;
};

/*
 * Event selector implemented using rejection-free KMC algorithm
 */
template <typename EventIDType, typename RateCalculatorType,
          typename EngineType = std::mt19937_64,
          typename GetImpactType = GetImpactFromTable<EventIDType>>
class RejectionFreeEventSelector
    : public EventSelectorBase<EventIDType, RateCalculatorType, EngineType> {
 public:
  // Construct given a rate calculator, event ID list, impact table, and random
  // number generator
  RejectionFreeEventSelector(
      const std::shared_ptr<RateCalculatorType> &rate_calculator_ptr,
      const std::vector<EventIDType> &event_id_list,
      const std::map<EventIDType, std::vector<EventIDType>> &impact_table,
      std::shared_ptr<RandomGeneratorT<EngineType>> random_generator =
          std::shared_ptr<RandomGeneratorT<EngineType>>())
      : EventSelectorBase<EventIDType, RateCalculatorType, EngineType>(
            rate_calculator_ptr, random_generator),
        event_rate_tree(event_id_list, this->calculate_rates(event_id_list)),
        impact_table(fill_impact_table(impact_table, event_id_list)),
        impacted_events_ptr(nullptr),
        get_impact(impact_table) {
    if (event_id_list.empty()) {
      throw std::runtime_error("Event ID list must not be empty.");
    }
  }

  // Construct given a rate calculator, event ID list, get impact function, and
  // random number generator
  RejectionFreeEventSelector(
      const std::shared_ptr<RateCalculatorType> &rate_calculator_ptr,
      const std::vector<EventIDType> &event_id_list, GetImpactType get_impact_f,
      std::shared_ptr<RandomGeneratorT<EngineType>> random_generator =
          std::shared_ptr<RandomGeneratorT<EngineType>>())
      : EventSelectorBase<EventIDType, RateCalculatorType, EngineType>(
            rate_calculator_ptr, random_generator),
        event_rate_tree(event_id_list, this->calculate_rates(event_id_list)),
        impact_table(),
        impacted_events_ptr(nullptr),
        get_impact(get_impact_f) {
    if (event_id_list.empty()) {
      throw std::runtime_error("Event ID list must not be empty.");
    }
  }

  // Select an event and return its ID and the time step
  std::pair<EventIDType, double> select_event() {
    // Because this function only selects events and does not process them,
    // it cannot update any rates impacted by the selected event until the next
    // call.
    update_impacted_event_rates();

    // Rates should now be updated. Calculate total rate and time step
    double _total_rate = event_rate_tree.total_rate();
    double time_step = this->calculate_time_step(_total_rate);

    // Query tree to select event
    double query_value =
        _total_rate * this->random_generator->sample_unit_interval();
    EventIDType selected_event_id = event_rate_tree.query_tree(query_value);

    // Update impacted event list and return
    set_impacted_events(selected_event_id);
    return std::make_pair(selected_event_id, time_step);
  }

  // Return total event rate, for events in the state before `select_event` is
  // called
  double total_rate() const { return event_rate_tree.total_rate(); }

  // Get the rate of a specific event
  double get_rate(const EventIDType &event_id) const {
    return event_rate_tree.get_rate(event_id);
  }

 private:
  // Tree storing event IDs and their corresponding rates
  EventRateTree<EventIDType> event_rate_tree;

  // Lookup table indicating, for a given event that is accepted, which events'
  // rates are impacted (optional, only used if impact_table is provided to
  // the constructor)
  const std::map<EventIDType, std::vector<EventIDType>> impact_table;

  // Pointer to vector of impacted events whose rates have not been updated
  mutable const std::vector<EventIDType> *impacted_events_ptr;

  // Function object to get impacted events from the accepted event ID
  GetImpactType get_impact;

  // Set the impact events pointer based on an accepted event ID
  void set_impacted_events(const EventIDType &accepted_event_id) {
    assert(impacted_events_ptr ==
           nullptr);  // pointer should be null before proceeding
    impacted_events_ptr = &get_impact(accepted_event_id);
    return;
  }

  // Update the stored rates for impacted events
  void update_impacted_event_rates() {
    if (impacted_events_ptr != nullptr) {
      for (const EventIDType &event_id : *impacted_events_ptr) {
        event_rate_tree.update_rate(event_id, this->calculate_rate(event_id));
      }
      impacted_events_ptr = nullptr;
    }
    return;
  }

  // Add missing event IDs to an impact table (with empty vectors as values)
  // and return it
  std::map<EventIDType, std::vector<EventIDType>> fill_impact_table(
      std::map<EventIDType, std::vector<EventIDType>> table_to_fill,
      std::vector<EventIDType> event_id_list) {
    for (const EventIDType &event_id : event_id_list) {
      table_to_fill[event_id];
    }
    return table_to_fill;
  }

  // Friend for testing
  friend class ::RejectionFreeEventSelectorTest;
};

}  // namespace lotto

#endif
