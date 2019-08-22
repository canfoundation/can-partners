#include <partnereward.hpp>

namespace eosio {
  
  constexpr symbol can_symbol = symbol("EOS", 4);
  
  
  ACTION partnereward::start( const asset & total_reward, uint32_t duration, uint32_t lock_duration)
  {
    require_auth(get_self());
    
    check(total_reward.is_valid(), "invalid reward");
    check(total_reward.amount > 0, "must input positive total of reward");
    check(total_reward.symbol == can_symbol, "symbol precision mismatch");
    
    check(duration > 0, "must set duration is positive number");
    check(lock_duration > 0, "must set lock_duration is positive number");
    
    _global.set(global{total_reward, current_time_point_sec(), 
                        duration, lock_duration}, get_self());
    
  }


  ACTION partnereward::addpartner(const name partner_account) 
  {
    require_auth(get_self());
    
    check(is_account(partner_account), "partner account account does not exist");
  
    partners partner_table(get_self(), get_self().value);
   
    auto partner = partner_table.find(partner_account.value);
  
    check(partner == partner_table.end(), "This partner has been existed!");
    
    partner_table.emplace(get_self(), [&](auto & p) {
  
        p.partner_name        = partner_account;
        p.last_claim_time     = current_time_point_sec();
        p.is_partner          = true;
    });
  }
  
  ACTION partnereward::banpartner(const name partner_account) {
   
    require_auth(get_self());
     
    partners partner_table(get_self(), get_self().value);
    
    auto partner = partner_table.find(partner_account.value);
  
    check(partner != partner_table.end(), "This partner does not exist!");
    
    partner_table.modify(partner, get_self(), [&](auto & p) {
        p.is_partner = false;
    });
  
  }
  
  ACTION partnereward::claimreward(const name partner_account)
  {
   
    require_auth(partner_account);
    
    asset reward;
    uint32_t total_month = 1;
    time_point_sec start_claim_time;
    
    partners partner_table(get_self(), get_self().value);
    
    auto partner = partner_table.find(partner_account.value);
    
    check(partner->is_partner, "Your account is banned!");
  
    auto config = _global.get();
    
    check_claim_time(partner->last_claim_time, config.start_at + config.lock_duration, config.duration);
    
    reward = calculate_reward(config.total_reward, config.lock_duration);
    
    update_claim_time( partner->last_claim_time, config.duration, total_month, start_claim_time);
    
    reward.amount *= total_month;
    
    // update totak reward for partner
    partner_table.modify(partner, get_self(), [&](auto & p) {
        p.last_claim_time = start_claim_time;
    });
  
    // Send reward to partner
    action(
      permission_level{ get_self(), "active"_n },
      "eosio.token"_n,"transfer"_n,
      std::make_tuple(get_self(), partner_account, reward,std::string("Partner receives reward montyly"))
    ).send();
  }
  
  /**
   * The formula for calculating reward per partner per month
   * partner_reward = total_lock_tokens / (TIME_LOCK_TOKEN)
   * 
   */
  
  asset inline  partnereward::calculate_reward( const asset& total_lock_tokens, 
                                                const uint32_t lock_duration) 
  {
  
    asset reward;
    int64_t amount_token = (total_lock_tokens.amount/lock_duration)*10000;
    
    reward = asset(amount_token, can_symbol);
    
    check( asset(0, can_symbol) < reward , "Nothing to be rewarded");
  
    return reward;
  }
  
  
  void  inline  partnereward::check_claim_time(const time_point_sec& last_claim_time, 
                                               const time_point_sec& time_expired,
                                               const uint32_t duration)
{
    time_point_sec current_time = current_time_point_sec();
    if ( current_time < ( last_claim_time + duration ) ) {
       std::string err = "Cannot claim reward yet, You still have "
                    + std::to_string( ( last_claim_time.sec_since_epoch() + duration ) - current_time.sec_since_epoch() )
                    + " seconds remaining";
      check( false, err.c_str() );
    }
    else if (last_claim_time > time_expired)
    {

      check( false, "Over 3 years to receive reward");
    }
    
  }
  
  /**
   * We calculate total of months to paid for partner and update time for next claim
   * For example: 3 months ago, they did not click to claim reward,
   *              total of months they will receive reward is 3 & we
   *              will update the remaining time for partner
   */
  void  inline partnereward::update_claim_time( const time_point_sec & last_claim_time,
                                                const uint32_t duration,
                                                uint32_t& total_month,
                                                time_point_sec & start_claim_time)
  {
        uint32_t month_delta = 0;
  
        time_point_sec time_delta   = current_time_point_sec() - (last_claim_time.sec_since_epoch() + duration);
        
        month_delta  = time_delta.sec_since_epoch()/ duration;
        
        if(month_delta > 0 ) {
          
          total_month += month_delta;
        }

        start_claim_time = current_time_point_sec() - (time_delta.sec_since_epoch() % duration);
  }

}// namespace eosio

EOSIO_DISPATCH(partnereward,(start)(addpartner)(banpartner)(claimreward))
