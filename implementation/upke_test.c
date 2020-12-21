
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#include "fp.h"
#include "csidh.h"
#include "classgroup.h"
#include "stdint.h"
#include "parameters.h"

void uint_print(uint const *x)
{
    for (size_t i = 8*LIMBS-1; i < 8*LIMBS; --i)
        printf("%02hhx", i[(unsigned char *) x->c]);
}

void fp_print(fp const *x)
{
    uint y;
    fp_dec(&y, x);
    uint_print(&y);
}

int main()
{
	clock_t t0, t1;

	init_classgroup();

	mpz_t priv_alice, priv_bob;
	public_key pub_alice, pub_bob;
	mpz_t alice_update;
	public_key pub_alice_updated;
	mpz_t priv_alice_updated;
	public_key shared_alice, shared_bob;

	printf("\n");

	t0 = clock();

	// Key Gen: 

	// Sample a random private key by sampling uniformly from the class group

	mpz_init(priv_alice);
	sample_mod_cn(priv_alice);
	
	
	// Apply the group action to convert to a public key

	private_key priva;
	mod_cn_2_vec(priv_alice, priva.e); // Convert the group element to a vector of exponents
	action(&pub_alice, &base, &priva); // Apply the action to the base curve E_0.

	// Do the same for Bob

	mpz_init(priv_bob);
	sample_mod_cn(priv_bob);
	

	// Apply the group action to convert to a public key

	private_key privb;
	mod_cn_2_vec(priv_bob, privb.e); // Convert the group element to a vector of exponents
	action(&pub_bob, &base, &privb); // Apply the action to the base curve E_0.


	// Update:

	// Bob generates an update and applies it to Alice's publi key. 

	// Generate update value

	mpz_init(alice_update);
	sample_mod_cn(alice_update);

	// Apply it to Alice's public key. 

	private_key privau;
	mod_cn_2_vec(alice_update, privau.e);
	assert(csidh(&pub_alice_updated, &pub_alice, &privau));

	// Alice needs to incorporate the update into her own private key.

	mpz_init(priv_alice_updated);
	mpz_add(priv_alice_updated, alice_update, priv_alice); 
	mpz_fdiv_r(priv_alice_updated, priv_alice_updated, cn);


	// Key Exchange:

	// Alice converts her private key to a vector and applies it to Bob's public key.

	private_key privab;
	mod_cn_2_vec(priv_alice_updated, privab.e);
	assert(csidh(&shared_alice, &pub_bob, &privab));

	private_key privba;
	mod_cn_2_vec(priv_bob, privba.e);
	assert(csidh(&shared_bob, &pub_alice_updated, &privba));

	t1 = clock();

	printf("Keygen and exchange took (%7.3lf ms):\n ", 1000. * (t1 - t0) / CLOCKS_PER_SEC);

	printf("    ");
    if (memcmp(&shared_alice, &shared_bob, sizeof(public_key)))
        printf("\x1b[31mNOT EQUAL!\x1b[0m\n");
    else
        printf("\x1b[32mequal.\x1b[0m\n");
    printf("\n");

    printf("\n");
    clear_classgroup();

}
