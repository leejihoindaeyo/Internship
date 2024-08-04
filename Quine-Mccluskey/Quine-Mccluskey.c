#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VARS 10
#define MAX_MINTERMS 1024

typedef struct {
    int minterm;
    int mask;
} Term;

typedef struct {
    Term terms[MAX_MINTERMS];
    int count;
} Group;

void print_binary(int num, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        printf("%d", (num >> i) & 1);
    }
}

void print_term(Term term, int bits) {
    for (int i = bits - 1; i >= 0; i--) {
        if (term.mask & (1 << i)) {
            printf("-");
        } else {
            printf("%d", (term.minterm >> i) & 1);
        }
    }
}

int count_ones(int num) {
    int count = 0;
    while (num) {
        count += num & 1;
        num >>= 1;
    }
    return count;
}

int combine_terms(Term a, Term b, Term *result) {
    int diff = a.minterm ^ b.minterm;
    if (count_ones(diff) == 1) {
        result->minterm = a.minterm & b.minterm;
        result->mask = a.mask | b.mask | diff;
        return 1;
    }
    return 0;
}

int is_covered(Term prime, Term minterm) {
    return (prime.minterm & ~prime.mask) == (minterm.minterm & ~prime.mask);
}

void find_prime_implicants(Group *groups, int num_vars, Group *prime_implicants) {
    int combined[MAX_MINTERMS] = {0};
    Group new_groups[MAX_VARS + 1] = {0};

    for (int i = 0; i < num_vars; i++) {
        for (int j = 0; j < groups[i].count; j++) {
            for (int k = 0; k < groups[i + 1].count; k++) {
                Term result;
                if (combine_terms(groups[i].terms[j], groups[i + 1].terms[k], &result)) {
                    new_groups[count_ones(result.minterm)].terms[new_groups[count_ones(result.minterm)].count++] = result;
                    combined[groups[i].terms[j].minterm] = 1;
                    combined[groups[i + 1].terms[k].minterm] = 1;
                }
            }
        }
    }

    for (int i = 0; i <= num_vars; i++) {
        for (int j = 0; j < groups[i].count; j++) {
            if (!combined[groups[i].terms[j].minterm]) {
                prime_implicants->terms[prime_implicants->count++] = groups[i].terms[j];
            }
        }
    }

    if (new_groups[0].count > 0) {
        find_prime_implicants(new_groups, num_vars, prime_implicants);
    }
}

void find_essential_prime_implicants(Group *prime_implicants, Term *minterms, int num_minterms, Group *essential_prime_implicants) {
    int covered[MAX_MINTERMS] = {0};

    for (int i = 0; i < prime_implicants->count; i++) {
        int cover_count = 0;
        for (int j = 0; j < num_minterms; j++) {
            if (!covered[minterms[j].minterm] && is_covered(prime_implicants->terms[i], minterms[j])) {
                cover_count++;
            }
        }
        if (cover_count == 1) {
            essential_prime_implicants->terms[essential_prime_implicants->count++] = prime_implicants->terms[i];
            for (int j = 0; j < num_minterms; j++) {
                if (is_covered(prime_implicants->terms[i], minterms[j])) {
                    covered[minterms[j].minterm] = 1;
                }
            }
        }
    }
}

int main() {
    int num_vars, num_minterms;
    printf("Enter the number of variables: ");
    scanf("%d", &num_vars);

    printf("Enter the number of minterms: ");
    scanf("%d", &num_minterms);

    Term minterms[MAX_MINTERMS];
    Group groups[MAX_VARS + 1] = {0};

    printf("Enter the minterms:\n");
    for (int i = 0; i < num_minterms; i++) {
        scanf("%d", &minterms[i].minterm);
        minterms[i].mask = 0;
        int ones = count_ones(minterms[i].minterm);
        groups[ones].terms[groups[ones].count++] = minterms[i];
    }

    Group prime_implicants = {0};
    find_prime_implicants(groups, num_vars, &prime_implicants);

    Group essential_prime_implicants = {0};
    find_essential_prime_implicants(&prime_implicants, minterms, num_minterms, &essential_prime_implicants);

    printf("Essential Prime Implicants:\n");
    for (int i = 0; i < essential_prime_implicants.count; i++) {
        print_term(essential_prime_implicants.terms[i], num_vars);
        printf("\n");
    }

    return 0;
}
