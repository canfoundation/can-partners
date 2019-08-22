**Smart contract for partner reward**
----------------------------------

Version 1.0.0

**1.  Policy**

Block Producer (BP) will initially pay a certain amount of tokens and have a three-year lock-up period.

The lock-up will be released at 1/36 rate per month.

Each month, BP claims thier reward manually. If this month, BP does not claim then CanChain will accumulate
to next month.

If a BP is banned (because they are not fit with CanChain checklist), they will be claimed tokens for previous month (they cannot claim for current month).

**2.  Funcionality**

**Start partner reward `start`**

This function helps CanChain stating a contract for BP reward with: total tokens which will be locked,

time to get reward (a month).

- `@param total_reward`: The total of CAT tokens which is locked for partner reward
- `@param duration`: The time for locked
- `@param lock_duration`: The time for receive token reward 

**Add a partner to CanChain `addpartner`**

After that, we must add all our BPs to CanChain, when a BP is added to this smart contract, it will be 

calculate time for claming rewarding.

- `@param partner_account`: The name of partner who will be added 

**Ban a partner from CanChain `banpartner`**

When a BP, they do not miss with CanChain requirement as time to produce block, security, etc, they will
be blocked by CanChain.

After BP is blocked, they cannot claim reward from current month.

- `@param partner_account`: The name of partner will be banned

**Block producer claims reward `claimreward`**

This smart contract will be calculate the amount of token which will be rewarded to BP. Therfore, in a month,

BP can claim their reward belonging CanChain's policy

- `@param partner_account`: The name of partner who will be received reward

**Example on jungle testnet using cleos**
----------------------------------

#### Get started


* Create account partnereward to deploy smart contract


##### Build & deploy smart contract

Go to partner_reaward directory then do flowing steps:

1.  Build smart contract

````bash
eosio-cpp -abigen -I include -contract partnereward -o partnereward.wasm src/partnereward.cpp
````

2. Deploy to jungle test net

````bash
$ cleos -u https://jungle2.cryptolions.io:443 set contract partnereward . partnereward.wasm partnereward.abi
````

##### To add permission on the jungle test net

````bash
$ cleos -u https://jungle2.cryptolions.io:443 set account permission partnereward active '{"threshold": 1,"keys": [{"key": "Pub_key","weight": 1}],"accounts": [{"permission":{"actor":"partnereward","permission":"eosio.code"},"weight":1}]}' owner -p partnereward
````

##### Create 7 partner accounts

We created 7 partner accounts on jungle test net via http://monitor.jungletestnet.io/#home

````bash
canpartner11
canpartner12
canpartner13
canpartner14
canpartner15
canpartner21
canpartner22
````

##### Start system via `start` action

````c++
start( const asset & total_reward, uint32_t duration, uint32_t lock_duration)
````
`The value of duration in seconds`

````bash
$ cleos -u https://jungle2.cryptolions.io:443 push action partnereward start '["1000 EOS", 86400, 36]' -p partnereward

````

##### Add partner to our smart contract via `addpartner` action
1. Add partner

````bash
$ cleos -u https://jungle2.cryptolions.io:443 push action partnereward addpartner '[canpartner11]' -p partnereward

$ cleos -u https://jungle2.cryptolions.io:443 push action partnereward addpartner '[canpartner12]' -p partnereward

$ cleos -u https://jungle2.cryptolions.io:443 push action partnereward addpartner '[canpartner13]' -p partnereward

$ cleos -u https://jungle2.cryptolions.io:443 push action partnereward addpartner '[canpartner14]' -p partnereward

$ cleos -u https://jungle2.cryptolions.io:443 push action partnereward addpartner '[canpartner15]' -p partnereward

$ cleos -u https://jungle2.cryptolions.io:443 push action partnereward addpartner '[canpartner21]' -p partnereward

$ cleos -u https://jungle2.cryptolions.io:443 push action partnereward addpartner '[canpartner22]' -p partnereward

````
2. Check result in partners table 


````bash
$ cleos -u https://jungle2.cryptolions.io:443 get table partnereward partnereward partners

````

##### Check `partnereward` account info 

````bash
$ cleos -u https://jungle2.cryptolions.io:443 get account partnereward
{
EOS balances:
     liquid:          200.0000 EOS
     staked:            2.0000 EOS
     unstaking:         0.0000 EOS
     total:           202.0000 EOS
}
````

##### Check `canpartner11` account info 

````bash
$ cleos -u https://jungle2.cryptolions.io:443 get account canpartner11
{
  EOS balances:
     liquid:          100.0000 EOS
     staked:            2.0000 EOS
     unstaking:         0.0000 EOS
     total:           102.0000 EOS
}
````

##### Check `canpartner12` account info 

````bash
$ cleos -u https://jungle2.cryptolions.io:443 get account canpartner12
{
  EOS balances:
     liquid:          100.0000 EOS
     staked:            2.0000 EOS
     unstaking:         0.0000 EOS
     total:           102.0000 EOS
}
````

##### Partner claim reward (For testing purpose only, we set time to claim is 1 hour)

1. Partner claim reward

````bash

$ cleos -u https://jungle2.cryptolions.io:443 push action partnereward claimreward '[canpartner11]' -p canpartner11

$ cleos -u https://jungle2.cryptolions.io:443 push action partnereward claimreward '[canpartner12]' -p canpartner12

````

2. Check result

````bash
$ cleos -u https://jungle2.cryptolions.io:443 get table partnereward partnereward partners

````
