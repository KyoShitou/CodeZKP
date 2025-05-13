#include "../src/FRI.hpp"
#include "../src/Field.hpp"
#include "../src/Polynomial.hpp"
#include <iostream>

using namespace std;

void commit(void* codeword) {
    vector<FieldElement>* codeword_vec = static_cast<vector<FieldElement>*>(codeword);
    cout << "Prover Commit: ";
    for (size_t i = 0; i != codeword_vec->size(); i++) {
        cout << (*codeword_vec)[i] << " ";
    }
    cout << std::endl;
}


void get_challenge(void* challenge) {
    FieldElement* challenge_ptr = static_cast<FieldElement*>(challenge);
    size_t alpha;
    cout << "Verifier Challenge: ";
    cin >> alpha;
    *challenge_ptr = FieldElement(static_cast<ttmath::ulint>(alpha));
}

void get_colinearity_challenge(void* index) {
    size_t* index_ptr = static_cast<size_t*>(index);
    cout << "Verifier Colinearity Challenge: ";
    cin >> *index_ptr;
}

void open_merkle(void* merkle) {
    // Placeholder for Merkle tree opening
    vector<FieldElement> open_points = *(vector<FieldElement>*)merkle;
    vector<FieldElement> open_points_x = {open_points[0], open_points[1], open_points[2]};
    vector<FieldElement> open_points_y = {open_points[3], open_points[4], open_points[5]};

    cout << "Doing colinearity test: " << (test_colinearity(open_points_x, open_points_y)? "Accept" : "Reject") << std::endl;
}

int test_complicated() {
    vector<string> co = {"187400097437367212389365372368488750623", "20366292988030266020033653491684411890", "32090773791144860259524506912754636167", "184139753753019779250381599663063797704", "163668746088420213065334478069665648746", "244377026572849212093466983909662578000", "98634904137557536937329106833517101778", "265592573055420709948869574794206159150", "99170084180712783522513900499270700494", "172816051812228688297837317114729370762", "69618572903465437935294542085892462352", "151407000241617249109547015544748170482", "201848738743860473355020082237990677063", "160775045997650719345610713443284996945", "237048354285006960028789946089217148413", "103428995089592038224786511250933437349", "258705002639141123740339572477906975865", "7169710631369110427706038959760074755", "189317327850018227851719344802034536402", "221185760298088133534457673570253582202", "178093899451823929821598276446578613174", "90768083055996661167128344102035504895", "151179709446425543603169899092463896955", "9214336528683649628315633251703818801", "210186727349492583044228020819835369098", "23777276077251071574014716528889728221", "97752900116944645536955251866743755892", "152132188437692484621456152265435515062", "245124285066621578163045693184387652269", "108867545216329180912183818917497194357", "159675270222951081506588736592684344921", "175887002578166714158599927980551564102", "27443769840055781783088689276711062258", "215850334474873502774037082161247533038", "24270314040728309291163385620945670077", "211890286003577145603816995700068134478", "95114860730080139726900173274943720365", "83416300762883793457468338788947159472", "65327177941190742329527883506355440949", "135464557125387599975864468952112379257", "154418812461449256661323019565563148338", "108047557185073637031492232440010777602"};
    vector<FieldElement> coeffs;
    for (size_t i = 0; i != co.size(); i++) {
        coeffs.push_back(FieldElement(co[i]));
    }
    Polynomial testPoly = Polynomial(coeffs);
    vector<FieldElement> codeword_fe;
    FieldElement omega = primitive_nth_root(256);
    FieldElement offset = generator();
    cout << "omega: " << omega << std::endl;
    cout << "offset: " << offset << std::endl;

    vector<FieldElement> FRI_domain;
    for (size_t i = 0; i != 256; i++) {
        codeword_fe.push_back(testPoly[(offset * (omega^i))]);
        FRI_domain.push_back(offset * (omega^i));
    }

    FRI::prove(codeword_fe, omega, offset, commit, get_challenge, get_colinearity_challenge, open_merkle);
}

int main() {
    vector<int> codeword;
    vector<FieldElement> codeword_fe;
    FieldElement omega = primitive_nth_root(32);
    FieldElement offset = generator();
    Polynomial testPoly = Polynomial(vector<FieldElement>{FieldElement(1), FieldElement(2), FieldElement(3), FieldElement(4), FieldElement(5)});
    cout << "omega: " << omega << std::endl;
    cout << "offset: " << offset << std::endl;
    cout << "Test polynomial: ";
    cout << testPoly << std::endl;
    cout << "degree: " << testPoly.degree() << std::endl;
    vector<FieldElement> FRI_domain;
    for (size_t i = 0; i != 32; i++) {
        codeword_fe.push_back(testPoly[(offset * (omega^i))]);
        FRI_domain.push_back(offset * (omega^i));
    }

    cout << "FRI domain: ";
    for (size_t i = 0; i != FRI_domain.size(); i++) {
        cout << FRI_domain[i] << " ";
    }

    FRI::prove(codeword_fe, omega, offset, commit, get_challenge, get_colinearity_challenge, open_merkle);
}
