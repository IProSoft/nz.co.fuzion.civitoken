<?php

function contact_civitoken_declare($token) {
  return [
    $token . '.zgoda_na_wykorzystanie_wizerunku' => 'Zgoda na wykorzystanie wizerunku (numeryczna)',
    $token . '.zgoda_na_komunikacje_elektroniczna' => 'Zgoda na komunikację elektroniczną (numeryczna)',
    $token . '.rodzaj_zadan_wolontariusza' => 'Rodzaje zadań wolontariusza ( lista ul )',
  ];
}

/**
 * Get Age tokens
 *
 * @param int $cid
 * @param array $value
 *
 * @return mixed
 */
function contact_civitoken_get($cid, &$value) {

  try{
    $contact = \Civi\Api4\Contact::get(FALSE)
      ->addSelect(
        'Zgody.zgoda_na_komunikacje_elektroniczna',
        'Zgody.zgoda_na_wykorzystanie_wizerunku',
        'Wolontariusz_informacje_dodatkowe.rodzaj_zadan'
      )
      ->addWhere('id', '=', $cid)
      ->execute()
      ->single();

    if(!empty($contact['Wolontariusz_informacje_dodatkowe.rodzaj_zadan'])) {
      $optionValues = \Civi\Api4\OptionValue::get(FALSE)
        ->addSelect('label')
        ->addWhere('option_group_id', '=', 126) //Wolontariusz_informacje_dodatkowe
        ->addWhere('value', 'IN', $contact['Wolontariusz_informacje_dodatkowe.rodzaj_zadan'])
        ->execute();

      if ($optionValues) {
        $value['contact.rodzaj_zadan_wolontariusza'] = '<ul>';
        foreach ($optionValues as $option) {
          $value['contact.rodzaj_zadan_wolontariusza'] .= sprintf('<li>%s</li>', $option['label']);
        }
        $value['contact.rodzaj_zadan_wolontariusza'] .= '</ul>';
      }

      $value['contact.zgoda_na_wykorzystanie_wizerunku'] = (int)$contact['Zgody.zgoda_na_wykorzystanie_wizerunku'];
      $value['contact.zgoda_na_komunikacje_elektroniczna'] = (int)$contact['Zgody.zgoda_na_komunikacje_elektroniczna'];
    }
  } catch (\Exception $e) {
    watchdog('dontus',
      'Problem z pobraniem wartości tokena: %msg',
      [
        '%cid' => $cid,
        '%msg' => $e->getMessage(),
      ],
      WATCHDOG_NOTICE
    );
    $value['contact.zgoda_na_wykorzystanie_wizerunku'] = $value['contact.zgoda_na_komunikacje_elektroniczna'] = $value['contact.rodzaj_zadan_wolontariusza'] = NULL;
  }

  return $value;
}
