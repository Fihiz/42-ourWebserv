#!/usr/bin/perl

use CGI ':standard';


print start_html('Gestion de formulaire'),
    h1('Gestion de formulaire'),
    start_form,
    "Quel est votre nom ? ",textfield('name'),
    p,
    "Que voulez-vous sur votre pizza?",
    p,
    checkbox_group(-name=>'toppings',
                   -values=>['fromage','tomate','jambon','champignons','oeuf','pepperoni'],
                   -defaults=>['fromage','tomate']),
    p,
    "Livraison ? ",
    popup_menu(-name=>'delivery',
               -values=>['a domicile','sur place',]),
    p,
    submit,
    end_form,
    hr;

if (param()) {
    print 
        "Votre nom est: ",em(param('name')),
        p,
        "Vous avez choisi: ",em(join(", ",param('toppings'))),
        p,
        "Livraison : ",em(param('delivery')),
        hr;
}

print end_html;