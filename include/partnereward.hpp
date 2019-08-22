#include <eosio/eosio.hpp>
#include <eosio/time.hpp>
#include <eosio/system.hpp>
#include <eosio/asset.hpp>
#include <eosio/singleton.hpp>


using namespace eosio;

namespace eosio
{

  CONTRACT partnereward : public eosio::contract {
    public:
      using contract::contract;
      partnereward(eosio::name receiver, eosio::name code, datastream<const char*> ds)
                : contract(receiver, code, ds), _global(get_self(), get_self().value){ }
  
      ACTION start(const asset & total_reward, uint32_t duration, uint32_t lock_duration);
      
      ACTION addpartner(const name partner_account);
      
      ACTION banpartner(const name partner_account);
      
      ACTION claimreward(const name partner_account);

    private:
    
      TABLE lock_partner {
        name            partner_name;
        time_point_sec  last_claim_time;
        bool            is_partner;
        
        uint64_t primary_key() const { return partner_name.value; }
        
      };

      TABLE global {
        asset           total_reward;
        time_point_sec  start_at;
        uint32_t        duration;       // 1 month
        uint32_t        lock_duration;  // 36 months      
      };


      typedef eosio::multi_index<"partners"_n, lock_partner> partners;
      typedef eosio::singleton<"config"_n, global> global_config;

  
      // constexpr static   uint32_t     REQUIRED_REWARD_DURATION   = 24*60*60;    // 1 days in second
      // constexpr static   uint32_t     EXPIRED_REWARD_IN_SECONDS  = 30*24*60*60; // 1 months in second
      
      global_config _global;
  
      static inline time_point_sec current_time_point_sec() {
            return time_point_sec(current_time_point());
      }
      
      asset inline  calculate_reward(const asset& total_lock_tokens, const uint32_t lock_duration);
      
      void  inline  check_claim_time(const time_point_sec& time_claim, 
                                      const time_point_sec& time_expired, const uint32_t duration);
                                      
      void  inline  update_claim_time(const time_point_sec & last_claim_time, const uint32_t duration,
                                        uint32_t& total_month, time_point_sec & start_claim_time);
      
  };

} // namespace eosio